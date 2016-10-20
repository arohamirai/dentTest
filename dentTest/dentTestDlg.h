
// dentTestDlg.h : 头文件
//

#pragma once
#include "opencv2/opencv.hpp"

// CdentTestDlg 对话框
class CdentTestDlg : public CDialogEx
{
// 构造
public:
	CdentTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	CvRect m_rectRoi;		//图像ROI，相对于图像自身的原点坐标
	float m_accuracy;
	float m_diameter;
	BOOL m_auto;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
