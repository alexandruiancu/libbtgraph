/*

 libbankingc++ - bank account transactions log analyzer

 Copyright (C) 2015  Alexandru Iancu <alexandru.iancu@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <libxml/parser.h>
//#include <libxml/tree.h>

#include "transactions_cfg.h"

TransactionsClassesCfg::TransactionsClassesCfg()
{
}

int TransactionsClassesCfg::load_classification(const char *szFilePath)
{
  if ( nullptr == szFilePath )
    return 1;

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

    xmlDocPtr pDoc = xmlReadFile(szFilePath, nullptr, 0);
  if ( nullptr == pDoc )
    return 2;
  xmlNodePtr pRoot = xmlDocGetRootElement(pDoc);
  const xmlChar *pRootNodeName = xmlCharStrdup("classes");
  if ( nullptr == pRoot || 0 != xmlStrcmp(pRoot->name, pRootNodeName) )
    {
      delete [] pRootNodeName;
      return 3;
    }
  delete [] pRootNodeName;
  std::vector<xmlNodePtr> arrClasses;
  for ( xmlNodePtr pChild = pRoot->children; nullptr != pChild; pChild = pChild->next )
    {
      if ( XML_ELEMENT_NODE != pChild->type )
	continue;
      arrClasses.push_back(xmlCopyNode(pChild, 1));
    }
  xmlFreeDoc(pDoc);
  xmlCleanupParser();

  if ( 0 != build_classes_tree(arrClasses) )
    return 4;
  return 0;
}

int TransactionsClassesCfg::build_classes_tree(std::vector<xmlNodePtr> &arrClasses)
{
  unsigned int nSize = arrClasses.size();
  if ( 0 == nSize )
    return 0;

  do
    {
      std::vector<xmlNodePtr>::iterator it = arrClasses.begin();
      while ( it != arrClasses.end() )
	{
	  xmlNodePtr pCrt = *it;
	  xmlNodePtr pMembers = pCrt->children;
	  if ( nullptr == pMembers || XML_ELEMENT_NODE != pMembers->type )
	    {
	      ++it;
	      continue;
	    }

	  bool bIncrement = true;
	  for ( xmlNodePtr pMember = pMembers->children; nullptr != pMember; )
	    {
	      if ( XML_ELEMENT_NODE != pMember->type )
		{
		  pMember = pMember->next;
		  continue;
		}
	      xmlChar *pHRefAttrName = xmlCharStrdup("href");
	      xmlChar *pHRef = xmlGetProp(pMember, pHRefAttrName);
	      delete [] pHRefAttrName;
	      if ( 0 == xmlStrlen(pHRef) )
		{
		  pMember = pMember->next;
		  continue;
		}

	      std::vector<xmlNodePtr>::iterator it2 = it;
	      xmlChar *pIdAttrName = xmlCharStrdup("id");
	      xmlChar *pId = xmlGetProp(*it2, pIdAttrName);
	      delete [] pIdAttrName;
	      while ( it != arrClasses.end() )
		{
		  if ( 0 == xmlStrcmp(pHRef+1/*skip '#'*/, pId) )
		    {
		      xmlAddChild(pMember, *it2);
		      arrClasses.erase(it2);
		      bIncrement = false;
		      break;
		    }
		}
	      if ( bIncrement )
		pMember = pMember->next;
	    }
	}
    }
  while ( nSize != arrClasses.size() );
  for ( std::vector<xmlNodePtr>::iterator it = arrClasses.begin(); it != arrClasses.end(); ++it )
    {
      m_arrRoots.push_back(new TransactionClass(*it));
      delete *it;
    }

  return 0;
}




