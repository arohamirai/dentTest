
// dentTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "dentTest.h"
#include "dentTestDlg.h"
#include "afxdialogex.h"
#include "CvvImage.h"
#include "halconcpp.h"

using namespace HalconCpp;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CdentTestDlg 对话框




CdentTestDlg::CdentTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CdentTestDlg::IDD, pParent)
	, m_accuracy(0.05)
	, m_diameter(0)
	, m_auto(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_srcImg = NULL;
	m_dstImg = NULL;
	m_rectRoi = cvRect(0,0,0,0);
}

void CdentTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT1,m_accuracy);
	DDX_Text(pDX,IDC_EDIT2,m_diameter);
}

BEGIN_MESSAGE_MAP(CdentTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SNAP, &CdentTestDlg::OnBnClickedSnap)
	ON_BN_CLICKED(IDC_LOADPIC, &CdentTestDlg::OnBnClickedLoadpic)
	ON_BN_CLICKED(IDC_MEASURE, &CdentTestDlg::OnBnClickedMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CdentTestDlg::OnEnKillfocusEdit1)
	ON_BN_CLICKED(IDC_RADIO1, &CdentTestDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CdentTestDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CdentTestDlg 消息处理程序

BOOL CdentTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CWnd *pWnd = GetDlgItem(IDC_PICTURE);
	pWnd->SetWindowPos(NULL,0,0,320,240,SWP_NOMOVE);

	CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO1);
	
	m_rectTracker.m_nStyle=CRectTracker::resizeOutside|CRectTracker::dottedLine;
	/*m_rect.left=0;
	m_rect.top=0;
	m_rect.right=100;
	m_rect.bottom=100;*/
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CdentTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{

		CDialogEx::OnPaint();

		CDC *pDC = GetDlgItem(IDC_PICTURE)->GetDC();
		HDC hDC = pDC->GetSafeHdc();
		CRect rect;
		GetDlgItem(IDC_PICTURE)->GetClientRect(&rect);
		CvvImage cImg;
		cImg.CopyOf(m_srcImg);
		cImg.DrawToHDC(hDC,&rect);

		ReleaseDC(pDC);


	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CdentTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CdentTestDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ

	CRect rectBox;
	CWnd *pWnd = GetDlgItem(IDC_PICTURE);
	pWnd->GetWindowRect(&rectBox);
	ScreenToClient(&rectBox);

	BOOL bIn = PtInRect(&rectBox,point);
	if (!bIn)  //在控件内才执行
	{
		//MessageBox(_T("不要超出图像区域，谢谢！"));
		Invalidate();
		UpdateWindow();
		CClientDC dc(this);
		HBRUSH hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
		HBRUSH oldBrush=(HBRUSH )dc.SelectObject(hBrush);
		HPEN hPen = CreatePen(PS_SOLID,1 , RGB( 255 , 0 , 0 ));
		HPEN oldPen = (HPEN)GetStockObject(NULL_PEN);

		dc.SelectObject(hPen);
		dc.SelectObject(hBrush);
		CRect rect = m_rectTracker.m_rect;
		//m_rectTracker.Draw(&dc);
		if (!m_auto)	dc.Ellipse(m_rectTracker.m_rect);

		DeleteObject(hBrush);
		DeleteObject(hPen);
		return;
	}
	//if (m_auto == TRUE)	// 自动测量
	
	int nRet = m_rectTracker.HitTest(point);

	if ( nRet < 0) //不在原有橡皮筋矩形框内，重画
	{
		Invalidate(TRUE);
		m_rectTracker.TrackRubberBand(this,point,FALSE);  //TrackRubberBand方法也是用于画外框.该方法会屏蔽掉父窗口的LButtonUp消息，即不响应父窗口的消息。

		CClientDC dc(this);
		HBRUSH hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
		HBRUSH oldBrush=(HBRUSH )dc.SelectObject(hBrush);
		HPEN hPen = CreatePen(PS_SOLID,1 , RGB( 255 , 0 , 0 ));
		HPEN oldPen = (HPEN)GetStockObject(NULL_PEN);

		dc.SelectObject(hPen);
		dc.SelectObject(hBrush);
		CRect rect = m_rectTracker.m_rect;
		m_rectTracker.Draw(&dc);
		if (!m_auto)	dc.Ellipse(m_rectTracker.m_rect);

		DeleteObject(hBrush);
		DeleteObject(hPen);
	}
	else 
	{
		if (nRet == CRectTracker::hitMiddle)//只有当鼠标在中间按下,才限制鼠标移动区域
		{
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(&pos);

			CRect rect;
			CWnd *pWnd = GetDlgItem(IDC_PICTURE);
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);

			CRect rectRubber = m_rectTracker.m_rect;

			rect.left += pos.x - rectRubber.left;
			rect.top  += pos.y - rectRubber.top;
			rect.right-=rectRubber.right-pos.x;
			rect.bottom-=rectRubber.bottom-pos.y;

			ClientToScreen(rect);
			ClipCursor(&rect);

		}else//在调整矩形时也不能调整到外面去.
		{
			CRect rect;
			CWnd *pWnd = GetDlgItem(IDC_PICTURE);
			pWnd->GetWindowRect(&rect);
			ClipCursor(&rect);
		}


		if (m_rectTracker.Track(this,point,FALSE,NULL)) //Track方法用于拉伸已经画好的外框的大小。
		{
			InvalidateRect(rectBox);
			UpdateWindow();

			CClientDC dc(this);

			HBRUSH hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
			HBRUSH oldBrush=(HBRUSH )dc.SelectObject(hBrush);
			HPEN hPen = CreatePen(PS_SOLID,1 , RGB( 255 , 0 , 0 ));
			HPEN oldPen = (HPEN)GetStockObject(NULL_PEN);

			dc.SelectObject(hPen);
			dc.SelectObject(hBrush);
			CRect rect = m_rectTracker.m_rect;
			m_rectTracker.Draw(&dc);
			if (!m_auto)	dc.Ellipse(m_rectTracker.m_rect);

			DeleteObject(hBrush);
			DeleteObject(hPen);

			ClipCursor(NULL);
		}

	}

	m_diameter = 0.00L;
	if (!m_auto)
	{
		long diameter = (m_rectTracker.m_rect.bottom + m_rectTracker.m_rect.right - m_rectTracker.m_rect.top - m_rectTracker.m_rect.left)/2;
		m_diameter = m_accuracy*diameter;
		UpdateData(FALSE);
		return;
	}
	//生成ROI
	CRect BoxRectInClient,rect;
	//CvRect rectRoi;		//图像ROI，相对于图像自身的原点坐标

	rect = m_rectTracker.m_rect;//橡皮筋的m_rect 的坐标原点是客户区坐标原点
	GetDlgItem(IDC_PICTURE)->GetWindowRect(&BoxRectInClient);
	ScreenToClient(&BoxRectInClient);

	m_rectRoi.x = rect.left - BoxRectInClient.left;
	m_rectRoi.y = rect.top - BoxRectInClient.top;

	m_rectRoi.width = rect.right - rect.left;
	m_rectRoi.height = rect.bottom - rect.top;
	
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CdentTestDlg::OnBnClickedSnap()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
}


void CdentTestDlg::OnBnClickedLoadpic()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	CFileDialog dlg(
		TRUE, _T("*.bmp|*.jpeg"), 
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image files (*.bmp; *.jpg;*.png) |*.bmp; *.jpg;*.jpeg;*.jge;*.png; ||"),
		NULL
		);                                        // 选项图片的约定
	dlg.m_ofn.lpstrTitle = _T("Open Image");    // 打开文件对话框的标题名
	if( dlg.DoModal() != IDOK )                    // 判断是否获得图片
		return;

	CString mPath = dlg.GetPathName();            // 获取图片路径

	int len =WideCharToMultiByte(CP_ACP,0,mPath,-1,NULL,0,NULL,NULL);  
	char *ptxtTemp =new char[len +1];  
	WideCharToMultiByte(CP_ACP,0,mPath,-1,ptxtTemp,len,NULL,NULL );  

	m_srcImg = cvLoadImage(ptxtTemp, CV_LOAD_IMAGE_COLOR );    // 读取图片
	if( !m_srcImg )                                    // 判断是否成功载入图片
		return;
	if( m_dstImg )                                // 对上一幅显示的图片数据清零
		cvZero( m_dstImg);
	delete[] ptxtTemp;  


	int w,h;
	w = m_srcImg->width;
	h = m_srcImg->height;

	CRect rectBox;
	GetDlgItem(IDC_PICTURE)->SetWindowPos(NULL,0,0,w,h,SWP_NOMOVE);
	GetDlgItem(IDC_PICTURE)->GetWindowRect(&rectBox);
	InvalidateRect(rectBox);
	UpdateWindow();

	CDC *pDC = GetDlgItem(IDC_PICTURE)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(IDC_PICTURE)->GetClientRect(&rect);
	CvvImage cImg;
	cImg.CopyOf(m_srcImg);
	cImg.DrawToHDC(hDC,&rect);

	ReleaseDC(pDC);

}


void CdentTestDlg::OnBnClickedMeasure()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	if (!m_auto)	return;
	action();
}

void CdentTestDlg::action()
{
	if (m_srcImg == NULL)
	{
		MessageBox(_T("请先载入图片，谢谢！"));
		return;
	}


	if(m_rectRoi.width == 0 || m_rectRoi.height == 0)
	{
		MessageBox(_T("请大致选择压痕所在区域，谢谢！"));
		return;
	}
	/************************************* 简    介 *************************************/
	/*          
	*
	*本算法用于自动测量压痕直径
	*算法的核心思想是使用高斯滤波器对图像进行滤波，根据压痕部分在频域内分布情况，自动定位压痕区域
	*然后进行一系列处理，得到压痕的轮廓，并进行圆拟合    
	*最终得到压痕的直径（像素宽度）
	*根据相机的分辨能力（一个像素对应实际尺寸的长度,单位：mm/像素，），可以将像素直径转换为实际物理直径（本算法没有进行转换）
	*
	*************************************************************************************/

	// **************   参数定义  *******************//
	// Local iconic variables
	//HImage ho_Image;
	HObject  ho_Image, ho_GaussFilter1, ho_GaussFilter2;
	HObject  ho_Filter, ho_ImageFFT, ho_ImageConvol, ho_ImageFiltered;
	HObject  ho_ImageResult, ho_RegionDynThresh, ho_ConnectedRegions;
	HObject  ho_SelectedRegions, ho_RegionUnion, ho_RegionClosing;
	HObject  ho_ConnectedRegions1, ho_SelectedRegions1, ho_ObjectSelected;
	HObject  ho_Contours, ho_Circle;

	// Local control variables
	HTuple  hv_Width, hv_Height, hv_Sigma1, hv_Sigma2;
	HTuple  hv_Min, hv_Max, hv_Range, hv_Area, hv_Row, hv_Column;
	HTuple  hv_Row1, hv_Column1, hv_Radius, hv_StartPhi, hv_EndPhi;
	HTuple  hv_PointOrder;
	// **************   参数定义完成  *******************//

	//  ***********  算法从这里开始 ****************  //
	//读取图片
	IplImage *grayImg = cvCreateImage(cvSize(m_srcImg->width,m_srcImg->height),IPL_DEPTH_8U,1);
	cvCvtColor(m_srcImg,grayImg,CV_RGB2GRAY);

	cvSetImageROI(grayImg,m_rectRoi);
	IplImage * pImage = cvCreateImage(cvSize(grayImg->roi->width,grayImg->roi->height),IPL_DEPTH_8U,1);
	cvCopy(grayImg,pImage);
	cvResetImageROI(grayImg);

	int height=pImage->height;
	int width=pImage->width;
	uchar *dataGray=new uchar[width*height];
	for(int i=0; i<height; i++)
	{
		memcpy(dataGray+width*i, pImage->imageData+pImage->widthStep*i,width);
	}
	GenImage1(&ho_Image,"byte",pImage->width,pImage->height,(Hlong)(dataGray));
	delete[ ] dataGray;

	//HImage ho_Image("byte",cvImg->width,cvImg->height,cvImg->imageData);
	//ReadImage(&ho_Image, "C:/Users/lenovo/Desktop/压痕20160423/测试4/孔4.bmp");
	cvShowImage("roi",pImage);
	//获取图片尺寸
	GetImageSize(ho_Image, &hv_Width, &hv_Height);

	//
	//Optimize the fft speed for the specific image size
	//加快傅里叶变换的速度（当图片长、宽是2的幂次方时，傅里叶变换的速度会较快，该算法的原理正是根据输入图片的尺寸，针对算法运行平台的CPU进行算法优化（这里是Intel的CPU））
	OptimizeRftSpeed(hv_Width, hv_Height, "standard");
	//
	//Construct a suitable filter by combining two gaussian
	//filters
	//本算法的核心
	//创建两个高斯滤波器
	//Sigma1，Sigma2参数是试出来的
	hv_Sigma1 = 10.0;
	hv_Sigma2 = 3.0;
	GenGaussFilter(&ho_GaussFilter1, hv_Sigma1, hv_Sigma1, 0.0, "none", "rft", hv_Width, 
		hv_Height);
	GenGaussFilter(&ho_GaussFilter2, hv_Sigma2, hv_Sigma2, 0.0, "none", "rft", hv_Width, 
		hv_Height);
	//两个高斯滤波器相减，得到一个带通滤波器,压痕的傅里叶变换后的频率落在此区域
	SubImage(ho_GaussFilter1, ho_GaussFilter2, &ho_Filter, 1, 0);
	//
	//Process the images iteratively

	//将彩色图像（三通道）转化为灰度图像（单通道）
	//Rgb1ToGray(ho_Image, &ho_Image);
	//Perform the convolution in the frequency domain
	//对图像进行傅里叶变换
	RftGeneric(ho_Image, &ho_ImageFFT, "to_freq", "none", "complex", hv_Width);
	//使用带通滤波器对傅里叶变换图像进行滤波，留下了的部分就包括压痕
	ConvolFft(ho_ImageFFT, ho_Filter, &ho_ImageConvol);
	//对滤波后的图像进行傅里叶反变换
	RftGeneric(ho_ImageConvol, &ho_ImageFiltered, "from_freq", "n", "real", hv_Width);
	//Process the filtered image

	//下面这三句语句目的是对图像进行阈值分割。
	//前两句是为了确定滤波后的压痕像素值与背景（其他部分）的像素值差异，然后自动挑选一个比较好的阈值
	//gray_range_rect：以当前像素中心，确定其周围10*10矩形区域内像素差的极值(max- min)
	GrayRangeRect(ho_ImageFiltered, &ho_ImageResult, 10, 10);
	//得到图像像素值的最大和最小值（整幅图像中像素值最大的那个值和最小的那个值）
	MinMaxGray(ho_ImageResult, ho_ImageResult, 0, &hv_Min, &hv_Max, &hv_Range);
	//阈值分割，区别出压痕与背景（其他部分）
	Threshold(ho_ImageResult, &ho_RegionDynThresh, (HTuple(5.55).TupleConcat(hv_Max*0.8)).TupleMax(), 
		255);

	//上面已经分割出压痕了，下面是一些处理，用来自动挑出压痕
	//连通域分析（使图像中属于同一个物体的像素在“逻辑”上连接起来，当成整体考虑，而不是一个一个像素，具体定义可百度或查找论文）
	Connection(ho_RegionDynThresh, &ho_ConnectedRegions);
	//根据面积挑选出疑似压痕的部分
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 4, 99999);

	//下面这些是一些图像形态学操作，目的是去掉一些小噪声
	Union1(ho_SelectedRegions, &ho_RegionUnion);
	//采用圆形结构元素 进行闭运算
	ClosingCircle(ho_RegionUnion, &ho_RegionClosing, 10);
	Connection(ho_RegionClosing, &ho_ConnectedRegions1);
	//去掉噪声后，重新挑选出疑似压痕的部分
	SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions1, "area", "and", 10, 99999);
	//获得疑似压痕部分的中心点坐标
	AreaCenter(ho_SelectedRegions1, &hv_Area, &hv_Row, &hv_Column);
	//挑选出压痕区域（打算根据压痕部分的中心点坐标来判断，即在放置样品时，使压痕处于中心，这样便可比较疑似压痕区域的中心点坐标是否处于图像中心，判断它是不是压痕）
	
	HTuple hv_num1;
	CountObj(ho_SelectedRegions1,&hv_num1);
	if (hv_num1.L() == 0)
	{
		MessageBox(_T("自动测试失败."));
		m_rectRoi = cvRect(0,0,0,0);
		return;
	}

	int center_x = m_rectRoi.width/2;
	int center_y = m_rectRoi.height/2;
	int index = 0;
	int MaxDis = INT_MAX;
	for (int i = 0; i < hv_Area.TupleLength(); ++i)
	{
		int r,c;
		r = hv_Row[i].D();
		c = hv_Column[i].D();
		double distance = (r-center_x)*(r-center_x) + (c-center_y)*(c-center_y);
		if (distance < MaxDis)
		{
			index = i;
			MaxDis = distance;
		}
	}

	SelectObj(ho_SelectedRegions1, &ho_ObjectSelected, index+1);

	HTuple hv_num2;
	CountObj(ho_SelectedRegions1,&hv_num2);
	if (hv_num2.L() == 0)
	{
		MessageBox(_T("自动测试失败."));
		m_rectRoi = cvRect(0,0,0,0);
		return;
	}

	//下面这两句是圆拟合
	//根据得到的压痕区域，提取他的边缘轮廓
	GenContourRegionXld(ho_ObjectSelected, &ho_Contours, "border");
	//边缘轮廓圆拟合，得到半径 Radius
	FitCircleContourXld(ho_Contours, "algebraic", -1, 0, 0, 3, 2, &hv_Row1, &hv_Column1, 
		&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);

	//在图像上画出拟合的圆
	GenCircle(&ho_Circle, hv_Row1, hv_Column1, hv_Radius);

	m_diameter = 2*hv_Radius[0].D()*m_accuracy;
	cvCircle(m_srcImg, cvPoint(m_rectRoi.x + hv_Column1[0].D(),m_rectRoi.y + hv_Row1[0].D()),hv_Radius[0].D(),CV_RGB(255,0,0));

	m_rectRoi = cvRect(0,0,0,0);
	OnPaint();
	UpdateData(FALSE);
}

void CdentTestDlg::OnEnKillfocusEdit1()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	CString str;
	str.Format(_T("%f"),m_accuracy);
	MessageBox(str);
}


void CdentTestDlg::OnBnClickedRadio1()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_auto = TRUE;
}


void CdentTestDlg::OnBnClickedRadio2()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_auto = FALSE;
}
