
// dentTestDlg.h : ͷ�ļ�
//

#pragma once
#include "opencv2/opencv.hpp"

// CdentTestDlg �Ի���
class CdentTestDlg : public CDialogEx
{
// ����
public:
	CdentTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


public:
	CRectTracker m_rectTracker;
	
	//CPoint m_point;

public:
	IplImage *m_srcImg;
	IplImage *m_dstImg;
	CvRect m_rectROI;
	
	
private:
	void action();

private:
	CvRect m_rectRoi;		//ͼ��ROI�������ͼ�������ԭ������
	float m_accuracy;
	float m_diameter;
	BOOL m_auto;
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedSnap();
	afx_msg void OnBnClickedLoadpic();
	afx_msg void OnBnClickedMeasure();
	afx_msg void OnEnKillfocusEdit1();
	
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
