#ifndef Color_h__
#define Color_h__
namespace DuiLib
{
	class UILIB_API DuiColor
	{
	public:
		DuiColor();
		DuiColor(int r, int g, int b, int a);
		DuiColor(DWORD rgba);
		static DuiColor* CreateInstance(COLORREF color);

		operator COLORREF();
		union
		{
			COLORREF   m_col;
			struct  
			{
				byte   r;
				byte   g;
				byte   b;
				byte   a;
			};
		};

		int   GetR();
		int   GetG();
		int   GetB();
		int   GetA();

		void  SetR(byte n);
		void  SetG(byte n);
		void  SetB(byte n);
		void  SetA(byte n);

		// ����ALPHAWֵ��ɫ����
		void  ReplaceRGB(COLORREF c);
		void  ReplaceRGB(BYTE r1, BYTE g1, BYTE b1);
		void  SetColor(COLORREF c);
		COLORREF  GetGDICompatibleValue();

		// ���ⲿʹ��Color����ʱ�����п�����ͨ��GetColor��ȡ����Color*ָ�룬Ҳ�п����ⲿ�Լ�ֱ������
		// һ��COLORREFֵ�����ʱ��͵�newһ��Color*��ʹ���߼���GetColor����һ�¡�Ϊ�˴ﵽ�ͷ��߼�һ
		// �µ�Ŀ�ģ����ͷź�����д��Release()
		long  AddRef();
		long  Release();

	private:
		long  lRef;
	};

	typedef DuiColor* DuiColorPtr;
	//
	// 9������������
	//
	class UILIB_API Image9Region
	{
	public:
		Image9Region(){ Set(0); }
		void Set( WORD w );
		void Set( WORD wLeftRight, WORD wTopBottom );
		void Set( WORD wLeft, WORD wTop, WORD wRight, WORD wBottom );

		WORD topleft;
		WORD top;
		WORD topright;
		WORD left;
		WORD right;
		WORD bottomleft;
		WORD bottom;
		WORD bottomright;
	};
}
#endif //Color_h__