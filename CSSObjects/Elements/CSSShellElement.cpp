// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- CSSShellElement.cpp : Implementation of CSSShellElement
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSShellElement.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSShellElement::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	CSSShellElement, CSSElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, CSS_Joint2dElement,
	CADSees
	| Product Desc : An OpenSees pre / post - processor
	| Company : Civil Soft Science
	| WEB Address : www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSShellElement::CSSShellElement() : CSSElement() {
	m_type = AcString(_T("Joint2d"));
}

CSSShellElement::CSSShellElement(int tag, std::vector<int> nodeTags)
	: CSSElement(tag, nodeTags, "Joint2d")
{
}

CSSShellElement::~CSSShellElement() {
}


//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean CSSShellElement::subWorldDraw(AcGiWorldDraw* mode) {
	assertReadEnabled();
	const Adesk::UInt32 faceListLength = 4;
	static const Adesk::Int32 faceList[] = { 4, 0, 1, 2, 3 };
	//pDeformedEntity->setColorIndex(DOCDATA->eleDfrmdColor);
	AcGiFaceData faceData;
	AcGiEdgeData edgeData;
	short colors[1];
	short edgeColors[4];
	if (ObjUtils::getShowDeformed())
	{
		colors[0] = eleDfrmdColor;
		for (int i = 0; i < 4; i++)
			edgeColors[i] = eleDfrmdColor;
		faceData.setColors(colors);
		edgeData.setColors(edgeColors);
		mode->geometry().shell(4, dfrmdVrtxList, faceListLength, faceList, &edgeData, &faceData);
		if (DISPOPTIONS.dispUndeformedWire)
		{
			//colors[0] = DOCDATA->wireColor;
			for (int i = 0; i < 4; i++)
				edgeColors[i] = wireColor;
			faceData.setColors(colors);
			edgeData.setColors(edgeColors);
			mode->geometry().shell(4, vrtxList, faceListLength, faceList, &edgeData, &faceData);
		}
	}
	else
	{
		colors[0] = wireColor;
		for (int i = 0; i < 4; i++)
			edgeColors[i] = wireColor;
		faceData.setColors(colors);
		edgeData.setColors(edgeColors);
		mode->geometry().shell(4, vrtxList, faceListLength, faceList, &edgeData, &faceData);
	}

	if (DISPOPTIONS.dispEleTags)
	{
		static AcGeVector3d vec, vec2, normal(0, 0, 1);
		vec = m_crds[3] - m_crds[1];
		vec2 = m_crds[2] - m_crds[0];
		AcGeVector3d up = vec.perpVector();
		AcGePoint3d crds = m_crds[0] + 0.5 * vec + 0.5 * vec2;
		AcString tagStr;
		tagStr.format(_T("%d"), m_tag);
		mode->geometry().text(crds, normal, AcGeVector3d(1, 0, 0), DISPOPTIONS.tagSize, 1., 0, tagStr.kACharPtr());
	}
	return (AcDbEntity::subWorldDraw(mode));
}


void CSSShellElement::subList() const
{
	CSSElement::subList();
}

bool CSSShellElement::updateGeometry(bool useDeformedGeom)
{
	assertWriteEnabled(false, true);
	bool res = CSSElement::updateGeometry(useDeformedGeom);
	if (!res)
		return false;
	static AcGeVector3d vec1, vec2;

	if (useDeformedGeom)
	{
		dfrmdVrtxList[0] = m_nodePtrs[0]->getDeformedCrds();
		dfrmdVrtxList[1] = m_nodePtrs[1]->getDeformedCrds();
		dfrmdVrtxList[2] = m_nodePtrs[2]->getDeformedCrds();
		dfrmdVrtxList[3] = m_nodePtrs[3]->getDeformedCrds();
	}
	else {
		vrtxList[0] = m_nodePtrs[0]->getCrds();
		vrtxList[1] = m_nodePtrs[1]->getCrds();
		vrtxList[2] = m_nodePtrs[2]->getCrds();
		vrtxList[3] = m_nodePtrs[3]->getCrds();
	}
	m_isNull = false;

	//m_nodePtrs[0]->setShiftVec(vec1);
	//m_nodePtrs[1]->setShiftVec(-vec2);
	m_nodePtrs[0]->close();
	m_nodePtrs[1]->close();
	m_nodePtrs[2]->close();
	m_nodePtrs[3]->close();

	m_isNull = false;
	return true;
}