#include "StdAfx.h"
#include "DuiReflection.h"

namespace DuiLib
{
	//************************************
	// ��������: RegistDuiClass
	// ��������: void
	// ������Ϣ: string _duiClassName					����
	// ������Ϣ: DuiInstanceFun _createDuiInstanceFun	ע��ĺ���ָ��
	// ����˵�����෴��ע�ᣬ���Ѿ�ע�������ֱ�ӷ���
	//************************************
	void DuiObjectMap::RegistDuiClass( string _duiClassName, DuiInstanceFun _createDuiInstanceFun )
	{
		if(GetDuiClassMaps().size() && GetDuiClassMaps().find(_duiClassName) != GetDuiClassMaps().end())
			return;

		GetDuiClassMaps()[_duiClassName] = _createDuiInstanceFun;
	}

	//************************************
	// ��������: createInstance
	// ��������: void*
	// ������Ϣ: string _duiClassName
	// ����˵����ͨ����������ʵ��
	//************************************
	void* DuiObjectMap::createInstance( string _duiClassName )
	{
		if(GetDuiClassMaps().size() && GetDuiClassMaps().find(_duiClassName) == GetDuiClassMaps().end())
		{
			std::map<string, DuiInstanceFun>& mDuiClassMaps = GetDuiClassMaps();
			DUITRACE(_T("û���ҵ������%S "),_duiClassName.c_str());
			return NULL;
		}

		return (GetDuiClassMaps()[_duiClassName])();
	}

	//************************************
	// ��������: createInstance
	// ��������: void*
	// ������Ϣ: LPCTSTR _duiClassName
	// ����˵����
	//************************************
	void* DuiObjectMap::createInstance( LPCTSTR _duiClassName )
	{
		char duiClassName[64];
		sprintf(duiClassName,"%S",_duiClassName);
		return createInstance(duiClassName);
	}

	//************************************
	// ��������: GetDuiClassMaps
	// ��������: std::map<string, DuiInstanceFun>&
	// ����˵��: 
	//************************************
	std::map<string, DuiInstanceFun>& DuiObjectMap::GetDuiClassMaps()
	{
		static std::map<string, DuiInstanceFun> m_duiClassMaps;
		return m_duiClassMaps;
	}

}

