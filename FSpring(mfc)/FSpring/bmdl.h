#pragma once
#include<iostream>
#include<afxwin.h>

inline std::ostream& operator<<(std::ostream& os, const CPoint& point) {
	os << "(" << point.x << "," << point.y << ")";
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const CRect& rect) {
	os << "[left : " << rect.left << "],[top : " << rect.top << "],[right : " << rect.right << "],[bottom : " << rect.bottom << "]";
	return os;
}
namespace bmdl {
	template<typename T>
	T Min(const T& a, const T& b) {
		return a < b ? a : b;
	}
	template<typename T>
	T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}
	template<typename T>
	void SetRange(T& val,const T& min_val, const T& max_val) {
		val = bmdl::Min(val, max_val);
		val = bmdl::Max(val, min_val);
	}
}
///CFont
namespace bmdl {
	//폰트의 실제 높이를 계산합니다.
	inline int MeasureFontHeight(CString font_str, int h, CDC* pdc) {
		int L = 0;
		int R = 1000;
		while (L <= R) {
			int M = (L + R) / 2;
			CFont font;
			font.CreatePointFont(M, font_str);
			CFont* old_font = pdc->SelectObject(&font);
			CSize sz;
			::GetTextExtentPoint32W(pdc->GetSafeHdc(), TEXT("A"), 1, &sz);
			if (sz.cy == h) {
				break;
			} else if (sz.cy > h) {
				R = M - 1;
			} else {
				L = M + 1;
			}
			pdc->SelectObject(old_font);
			font.DeleteObject();
		}
		return (L + R) / 2;
	}
	inline int MeasureFontHeight2(CString font_str,CString str, int default_height, int fit_width, CDC* pdc) {
		CSize sz;
		do {
			CFont font;
			font.CreatePointFont(default_height, font_str);
			CFont* old_font = pdc->SelectObject(&font);
			
			::GetTextExtentPoint32W(pdc->GetSafeHdc(), str, str.GetLength(), &sz);
			if (sz.cx > fit_width) {
				default_height--;
			}
		} while (sz.cx > fit_width);
		return default_height;
	}
}
///Double Buffering
namespace bmdl {
	class DBB {
	protected:
		CDC* m_pdc;
		CRect m_rect;
		CDC m_dc;
		CBitmap m_cbmp;
		CBitmap* m_cbmp_old;
	public:
		CDC& getDC() {
			return this->m_dc;
		}
		CDC* getPDC() {
			return &this->m_dc;
		}
		DBB(CDC* pdc, CRect rect) {
			this->m_pdc = pdc;
			this->m_rect = rect;
			this->m_dc.CreateCompatibleDC(pdc);
			this->m_cbmp.CreateCompatibleBitmap(pdc, rect.Width(), rect.Height());
			this->m_cbmp_old = (CBitmap*)this->m_dc.SelectObject(&this->m_cbmp);
			this->m_dc.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS);
		}
		static CRect Normalize(CRect rect) {
			rect.NormalizeRect();
			rect.OffsetRect(-rect.left, -rect.top);
			return rect;
		}
		void Draw() {
			this->m_pdc->BitBlt(this->m_rect.left, this->m_rect.top, this->m_rect.Width(), this->m_rect.Height(), &this->m_dc, 0, 0, SRCCOPY);
			this->m_dc.SelectObject(this->m_cbmp_old);
			this->m_dc.DeleteDC();
			this->m_cbmp.DeleteObject();
		}
	};
}