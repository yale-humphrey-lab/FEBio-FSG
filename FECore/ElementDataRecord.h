#pragma once
#include "FECoreBase.h"
#include "DataRecord.h"

class FEElement;
class FEElementSet;

//-----------------------------------------------------------------------------
//! Base class for element log data
class FECORE_API FELogElemData : public FECoreBase
{
	FECORE_SUPER_CLASS

public:
	FELogElemData(FEModel* fem);
	virtual ~FELogElemData();
	virtual double value(FEElement& el) = 0;
};

//-----------------------------------------------------------------------------
class FECORE_API ElementDataRecord : public DataRecord
{
	struct ELEMREF
	{
		int	ndom;
		int	nid;
	};

public:
	ElementDataRecord(FEModel* pfem, const char* szfile);
	double Evaluate(int item, int ndata);
	void Parse(const char* sz);
	void SelectAllItems();
	int Size() const;
	void SetItemList(FEElementSet* pg);

protected:
	void BuildELT();

protected:
	vector<ELEMREF>	m_ELT;
	int				m_offset;
	vector<FELogElemData*>	m_Data;
};
