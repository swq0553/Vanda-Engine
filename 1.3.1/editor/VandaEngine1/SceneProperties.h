//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once
#include "afxwin.h"


// CSceneProperties dialog

class CSceneProperties : public CDialog
{
	DECLARE_DYNAMIC(CSceneProperties)

public:
	CSceneProperties(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSceneProperties();

// Dialog Data
	enum { IDD = IDD_DIALOG_SCENE_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_checkBoxPlayAnimation;
	CButton m_checkBoxLoopAnimation;
	CComboBox m_comboAnimationClip;
	virtual BOOL OnInitDialog();
	CVoid SetScene(CScene* scene);
	CScene* m_scene;
protected:
	virtual void OnOK();
public:
	afx_msg void OnCbnSelchangeComboAnimationClip();
	CInt m_currentAnimClip;
};
