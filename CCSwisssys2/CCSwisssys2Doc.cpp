
// CCSwisssys2Doc.cpp : implementation of the CCCSwisssys2Doc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CCSwisssys2.h"
#endif

#include "CCSwisssys2Doc.h"

#include <propkey.h>
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCCSwisssys2Doc

IMPLEMENT_DYNCREATE(CCCSwisssys2Doc, CDocument)

BEGIN_MESSAGE_MAP(CCCSwisssys2Doc, CDocument)
END_MESSAGE_MAP()


// CCCSwisssys2Doc construction/destruction

CCCSwisssys2Doc::CCCSwisssys2Doc()
{
	// TODO: add one-time construction code here

}

CCCSwisssys2Doc::~CCCSwisssys2Doc()
{
}

BOOL CCCSwisssys2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	ratings_file = "";
	constant_contact_file = "";
	school_code_file = "";
	sections.clear();
	mrplayers.clear();

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CCCSwisssys2Doc serialization

void CCCSwisssys2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << ratings_file;
		ar << constant_contact_file;
		ar << school_code_file;
		sections.Serialize(ar);
		mrplayers.Serialize(ar);
	}
	else
	{
		ar >> ratings_file;
		ar >> constant_contact_file;
		ar >> school_code_file;
		sections.Serialize(ar);
		mrplayers.Serialize(ar);
	}
}

CString getGradeString(wchar_t grade) {
	switch (grade) {
	case 'A': return CString("A - Kindergarten");
	case 'B': return CString("B - 1st Grade");
	case 'C': return CString("C - 2nd Grade");
	case 'D': return CString("D - 3rd Grade");
	case 'E': return CString("E - 4th Grade");
	case 'F': return CString("F - 5th Grade");
	case 'G': return CString("G - 6th Grade");
	case 'H': return CString("H - 7th Grade");
	case 'I': return CString("I - 8th Grade");
	case 'J': return CString("J - 9th Grade");
	case 'K': return CString("K - 10th Grade");
	case 'L': return CString("L - 11th Grade");
	case 'M': return CString("M - 12th Grade");
	case 'N': return CString("N - Adult");
	default:
		assert(0);
		return CString("Unknown grade.");
	}
}

CString getSectionTypeString(int type) {
	switch (type) {
	case SWISS: return CString("Swiss");
	case ROUND_ROBIN: return CString("Round Robin");
	default:
		assert(0);
		return CString("Unknown section type.");
	}
}

std::vector< ConstantContactEntry > load_constant_contact_file(const std::wstring &filename) {
	auto ccret = load_csvw_file(filename, true);

	std::vector< ConstantContactEntry > ret;
	unsigned i;
	for (i = 0; i < ccret.size(); ++i) {
		if (ccret[i][23] == L"Registered") {
			ret.push_back(ConstantContactEntry(ccret[i]));
		}
	}
	return ret;
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CCCSwisssys2Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CCCSwisssys2Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CCCSwisssys2Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCCSwisssys2Doc diagnostics

#ifdef _DEBUG
void CCCSwisssys2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCCSwisssys2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCCSwisssys2Doc commands
