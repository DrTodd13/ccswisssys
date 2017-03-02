
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

std::vector< ConstantContactEntry > load_constant_contact_file(const std::wstring &filename, int * locations) {
	auto ccret = load_csvw_file(filename, true);

	std::vector< ConstantContactEntry > ret;
	unsigned i;
	for (i = 0; i < ccret.size(); ++i) {
		if (ccret[i][locations[REGISTERED]] == L"Registered") {
			ret.push_back(ConstantContactEntry(ccret[i], locations));
		}
	}
	return ret;
}

int allstars2016[] = { 0, 1, 11, 13, 12, 15, 17, 22 };

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

int LevenshteinDistance(const std::wstring &s, const std::wstring &t) {
	std::wstring supper, tupper;

	supper = toUpper(s);
	tupper = toUpper(t);

	// degenerate cases
	if (supper == tupper) return 0;
	unsigned tlen = t.length();
	unsigned slen = s.length();

	if (slen == 0) return tlen;
	if (tlen == 0) return slen;

	unsigned v0len = tlen + 1;
	unsigned v1len = tlen + 1;

	// create two work vectors of integer distances
	int *v0 = new int[v0len];
	int *v1 = new int[v1len];

	// initialize v0 (the previous row of distances)
	// this row is A[0][i]: edit distance for an empty s
	// the distance is just the number of characters to delete from t
	for (int i = 0; i < v0len; i++)
	   v0[i] = i;

	for (int i = 0; i < slen; i++)
	{
		// calculate v1 (current row distances) from the previous row v0

		// first element of v1 is A[i+1][0]
		//   edit distance is delete (i+1) chars from s to match empty t
		v1[0] = i + 1;

		// use formula to fill in the rest of the row
		for (int j = 0; j < tlen; j++)
		{
			int cost = (supper[i] == tupper[j]) ? 0 : 1;
			v1[j + 1] = min(min(v1[j] + 1, v0[j + 1] + 1), v0[j] + cost);
		}

		// copy v1 (current row) to v0 (previous row) for next iteration
		for (int j = 0; j < v0len; j++)
			v0[j] = v1[j];
	}

	return v1[min(slen,tlen)];
}

std::wstring toUpper(const std::wstring &s) {
	std::wstring ret = s;

	for (unsigned j = 0; j < s.size(); ++j) {
		ret[j] = toupper(ret[j]);
	}

	return ret;
}

// CCCSwisssys2Doc commands
