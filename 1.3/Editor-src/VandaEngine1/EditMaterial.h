//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CEditMaterial dialog

class CEditMaterial : public CDialog
{
	DECLARE_DYNAMIC(CEditMaterial)

public:
	CEditMaterial(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditMaterial();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_MATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_editBoxNormalMap;
	CRichEditCtrl m_editBoxGlossMap;
	CRichEditCtrl m_editBoxAlphaMap;
	CRichEditCtrl m_editBoxShadowMap;
	CRichEditCtrl m_editBoxDirtMap;
	CRichEditCtrl m_editBoxDiffuse;
	CButton m_btnNormalMap;
	CButton m_btnRemoveNormalMap;
	CButton m_btnGlossMap;
	CButton m_btnAlphaMap;
	CButton m_btnShadowMap;
	CButton m_btnDirtMap;
	CButton m_btnDiffuse;
	CButton m_btnRemoveGlossMap;
	CButton m_btnRemoveAlphaMap;
	CButton m_btnRemoveShadowMap;
	CButton m_btnRemoveDirtMap;
	CButton m_btnRemoveDiffuse;
	CString m_strPercentage;
	CInt m_iPercentage;
	CString m_strDensity;
	CFloat m_fDensity;
	afx_msg void OnBnClickedBtnNormalmap();

	CVoid SetEditBoxNormalMapName( CString string );
	CVoid SetEditBoxGlossMapName( CString string );
	CVoid SetEditBoxAlphaMapName( CString string );
	CVoid SetEditBoxShadowMapName( CString string );
	CVoid SetEditBoxDirtMapName( CString string );
	CVoid SetEditBoxDiffuseName( CString string );

	CVoid SetEditBoxBias( CFloat bias );
	CVoid SetEditBoxScale( CFloat scale );
	CVoid SetPhysX( CInstanceGeometry* instanceGeo );
	CBool m_normalBtnState;
	CBool m_glossBtnState;
	CBool m_alphaBtnState;
	CBool m_shadowBtnState;
	CBool m_dirtBtnState;
	CBool m_diffuseBtnState;
	CBool m_physXBtnState;

	CVoid SetNormalBtnState( CBool state );
	CVoid SetGlossBtnState( CBool state );
	CVoid SetAlphaBtnState( CBool state );
	CVoid SetShadowBtnState( CBool state );
	CVoid SetDirtBtnState( CBool state );
	CVoid SetDiffuseBtnState( CBool state );

	CBool m_removeNormalBtnState;
	CBool m_removeGlossBtnState;
	CBool m_removeAlphaBtnState;
	CBool m_removeShadowBtnState;
	CBool m_removeDirtBtnState;
	CBool m_removeDiffuseBtnState;
	CBool m_removePhysXBtnState;

	CVoid SetRemoveNormalBtnState( CBool state );
	CVoid SetRemoveGlossBtnState( CBool state );
	CVoid SetRemoveAlphaBtnState( CBool state );
	CVoid SetRemoveShadowBtnState( CBool state );
	CVoid SetRemoveDirtBtnState( CBool state );
	CVoid SetRemoveDiffuseBtnState( CBool state );

	CInstanceGeometry* m_instanceGeometry;
public:
	CString m_strDiffuseName, m_strPureDiffuseName; //To get the name of the diffuse texture from a dialog box
	CString m_strAlphaMapName, m_strPureAlphaMapName;  //To get the name of the alpha map from a dialog box
	CString m_strGlossMapName, m_strPureGlossMapName;  //To get the name of the gloss map from a dialog box
	CString m_strNormalMapName, m_strPureNormalMapName; //To get the name of the normal map from a dialog box
	CString m_strShadowMapName, m_strPureShadowMapName; //To get the name of the shadow map from a dialog box
	CString m_strDirtMapName, m_strPureDirtMapName; //To get the name of the shadow map from a dialog box

	CFloat m_parallaxMapBias, m_parallaxMapScale;
	CString m_strParallaxMapBias, m_strParallaxMapScale;

	CString m_strPhysX;
	afx_msg void OnBnClickedBtnGlossmap();
	afx_msg void OnBnClickedBtnAlphamap();
	afx_msg void OnBnClickedBtnShadowmap();
	afx_msg void OnBnClickedBtnDirtmap();
	afx_msg void OnBnClickedBtnDiffuse();
	afx_msg void OnBnClickedBtnDeleteNormalmap();
	afx_msg void OnBnClickedBtnDeleteGlossmap();
	afx_msg void OnBnClickedBtnDeleteAlphamap();
	afx_msg void OnBnClickedBtnDeleteShadowmap();
	afx_msg void OnBnClickedBtnDeleteDirtmap();
	afx_msg void OnBnClickedBtnDeleteDiffuse();

	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnEnChangeEditNormalmapBias();
	afx_msg void OnEnChangeEditNormalmapScale();
	CEdit m_editBoxScale;
	CEdit m_editBoxBias;
	CBool m_firstBias, m_firstScale;
	afx_msg void OnBnClickedBtnPhysx();
	afx_msg void OnBnClickedBtnDeletePhysx();
	CVoid RemovePhysXMesh();
	CRichEditCtrl m_editBoxPhysX;
	CButton m_btnRemovePhysX;
	CButton m_btnGeneratePhysX;
	CComboBox m_comboPhysX;
	afx_msg void OnBnClickedBtnPhysxProperties();
	afx_msg void OnCbnSelchangeComboPhysx();
	afx_msg void OnEnChangeEditPhysxPercentage();
	afx_msg void OnEnChangeEditPhysxDensity();
	void RemoveUnsharedImage( CString string, CTextureMapType type);
	CRichEditCtrl m_physXPercentage;
	CRichEditCtrl m_physXDensity;
	CButton m_checkBoxTrigger;
};
