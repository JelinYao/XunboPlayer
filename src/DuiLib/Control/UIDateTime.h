#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__
#pragma once

namespace DuiLib
{
	typedef struct tag_CalendarStyle
	{
		CDuiString	nCalenderBorderColor;		//�����߿���ɫ
		CDuiString	nCalendarBkColor;			//����������ɫ
		CDuiString	nMainTitleBkColor;			//���������ⱳ����ɫ
		CDuiString	nSubTitleBkColor;			//���������ⱳ����ɫ
		CDuiString	nWeekendColorA;				//��ĩ�����ڸ��б�����ɫ
		CDuiString	nWeekendColorB;				//��ĩ�����ڸ��б�����ɫ
		CDuiString	nDayHotColor;				//���ڻ�ý���ʱ������ɫ
		CDuiString	nDayPushedColor;			//���ڱ�����ʱ������ɫ
		CDuiString	nDaySelectColor;			//���ڱ�ѡ��ʱ������ɫ
		CDuiString	nDayDisabledColor;			//���ڱ�����ʱ�ı���ɫ
		CDuiString	nNoCurMonthDayColor;		//�Ǳ������ڵı�����ɫ
		CDuiString	nWeekIntervalColorA;		//�ܸ�����ɫA
		CDuiString	nWeekIntervalColorB;		//�ܸ�����ɫB
		CDuiString	nStatusBkColor;				//�ײ���Ϣ������ɫ
		CDuiString	nWeekEndTextColor;			//��ĩ����ɫ
		CDuiString	nWeekTextColor;				//��һ����5����ɫ �����ⲿ��
		CDuiString  nTextColor;					//�������� ������ɫ
	}TCalendarStyle;

	typedef struct tag_SubTitleString
	{
		CDuiString	nSundayStr;
		CDuiString	nMondayStr;
		CDuiString	nTuesdayStr;
		CDuiString	nWednesdayStr;
		CDuiString	nThursdayStr;
		CDuiString	nFridayStr;
		CDuiString	nSaturdayStr;
		CDuiString	nToDayString;
	}TSubTitleString;

	typedef struct UILIB_API tag_CalendarInfo
	{
		int		nYear;
		int		nMooth;
		int		nDay;
		int		nAsMooth;
		int		nWeek;
		int		nWeekLine;
	}TCalendarInfo;

	class UILIB_API CCalendarUI : public CVerticalLayoutUI
	{
	public:
		CCalendarUI(void);
		~CCalendarUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	DoEvent(TEventUI& event);
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		CControlUI* CreateWeekPanel(CControlUI* _Panent,int _Week);
		CControlUI* CreateDayPanel(int _Week,LPCTSTR _GroupName);
		void InitWeekPanel(CControlUI* _Control,int _Week);
		void InitDayPanel( CControlUI* _Control,bool bWeekEnd,TCalendarInfo& _CalendarInfo );
		void InitDayPanel( CControlUI* _Control,bool bWeekEnd,int _Year,int _Mooth,int _Day,int _Week,int _WeekLine,int _AsMooth );
	public:
		bool IsLeapYear(int _Year);
		int  GetNumLeapYear(int _Year);
		int  DaysOfMonth(int _Mooth,int _Year = -1);
		void InitCalendarDis(int _Year,int _Month);
		void CalDateTime(TCalendarInfo* _TCalendarInfo);
		void CalDateTime(int _Year,int _Mooth,int _Day);
		TCalendarInfo*	GetDateTime() const;
		void SetDateTime(TCalendarInfo* tm);
	public:
		bool SetSubTitleString(LPCTSTR _Name);
		void SetSubTitleString(TSubTitleString& _SubTitleString);
		void SetCalendarStyle(TCalendarStyle& _TCalendarStyle);
		bool AddSubTitleString(LPCTSTR _Name,TSubTitleString& _SubTitleString);
		bool AddSubTitleString(LPCTSTR _Name,LPCTSTR _Sunday,LPCTSTR _Monday,LPCTSTR _Tuesday,LPCTSTR _Wednesday,LPCTSTR _Thursday,LPCTSTR _Friday,LPCTSTR _Saturday);
		TSubTitleString& GetSubTitleString(LPCTSTR _Name = NULL);
		bool RemoveAtSubTitleString(LPCTSTR _Name);
		void RemoveAllSubTitleString();

	public:
		bool OnToday(void* param);
		bool OnSelcetDay( void* param );
		bool OnSelectYear(void* param);
		bool OnSelectMonth(void* param);
		void SetDateTimeControl(void* pControl);

	public:

		void AnyCalendarStyle(LPCTSTR _StyleStr,TCalendarStyle* _TCalendarStyle = NULL);
		void InitYearData();//��ʼ������
		void InitMonthData();//��ʼ������

		bool IsCobWnd(HWND hWnd);
		bool IsClose();

		CComboUI* GetYearCob(){return m_pcob_year;}
		CComboUI* GetMonthCob(){return m_pcob_month;}
		CButtonUI* GetToDayBtn(){return m_pbtn_today;}
		void InitWeekData(int nIndex,LPCTSTR pstrValue);

	private:
		CHorizontalLayoutUI*	m_pMainTitleHoriz;//���ⲿ������
		CHorizontalLayoutUI*	m_pMainTitleLeft;//�������
		CHorizontalLayoutUI*	m_pMainTitleRight;//�ұ�����
		CLabelUI*				m_plbl_year;
		CLabelUI*				m_plbl_month;
		CComboUI*				m_pcob_year;//��
		CComboUI*				m_pcob_month;//��
		CButtonUI*				m_pbtn_today;//���ؽ���

		CHorizontalLayoutUI*	m_pSubTitleHoriz;//���ڼ�����
		CHorizontalLayoutUI*    m_pWeekPanelHoriz;//��������
		CVerticalLayoutUI*		m_pWeekPanelVert;//��������

		CDuiString				m_szCurData;//��ȡ��ǰ����
		CDuiString				m_szYCurData;//���� ����
		int						m_nCurYear;//��ǰѡ����
		int						m_nCurMooth;//��
		int						m_nCurDay;//��

		int						m_Today;
		struct tm*				m_pCurDateTime;

		TCalendarStyle			m_DefaultStyle;
		TSubTitleString			m_SubTitleString;
		CStdPtrArray			m_TCalendarInfoArray;
		CStdStringPtrMap		m_SubTitleStringArray;

		int m_nWidth;
		int m_nHeight;

		bool m_bIsClose;

		void* m_pDateTime;
		TCalendarInfo*			m_pCalendar;
	};

	class CCalendarWnd;
	/// ʱ��ѡ��ؼ�
	class UILIB_API CDateTimeUI : public CButtonUI
	{
		friend class CCalendarWnd;
	public:
		CDateTimeUI();
		~CDateTimeUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);

		void SetDateTimeWnd(HWND hWnd);
		HWND GetDateTimeWnd();

		bool Activate();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		
		void SetDateTime(struct tm* pCurDateTime);
		TCalendarInfo* GetDateTime();

		CDuiString Format(CDuiString szFormat);

		void SetCalendarStyle(LPCTSTR pstrValue);
		CDuiString GetCalendarStyle() const;
		void SetCalendarAttr(LPCTSTR pstrValue);
		CDuiString GetCalendarAttr() const;

		void SetFormatStyle(int nStyle);

		void CloseWnd();
		

	protected:
		CCalendarWnd* m_pWindow;
		HWND m_hDateTimeWnd;

		TCalendarInfo*			m_pCurDateTime;
		CDuiString				m_szCalendarAttr;
		CDuiString				m_szCalendarStyle;
		int						m_nFormatStyle;//��ʽ����ʽ
		CDuiString				m_szFormatText;
	};
}
#endif // __UIDATETIME_H__