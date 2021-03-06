#pragma once
#include <list>
#include <string>
#include <set>
#include <vector>
#include "types.h"

class Transaction
{
 public:
  Transaction();
  int initialize();
  int set_primary_attributes(const vs &vAttrs);
  int set_primary_attribute_values(const vs &vAttrVals);
  int add_attributes(const std::vector<vs> &vAttrVals);
  int get_credit_index(int &nIndex);
  int get_transaction_details(std::string &sTransactionDetails);
  virtual int as_xml(std::string &sOut);

  std::vector<ps> m_primary_attrs;
  ss m_attrs;

 protected://helpers
  int as_xml_primary_attributes(std::string &sOut);
  int as_xml_attributes(std::string &sOut);
  int escapeWhiteSpace(const std::string &sIn, std::string &sOut);
};

class IsCreditTransaction
{
  int m_nCreditAttributeIndex;
 public:
  IsCreditTransaction(int nIndex);
  bool operator()(Transaction *pT) const;
};


typedef std::vector<Transaction *> Transactions;






