#include "pch.h"
#include "framework.h"
#include "p2p-send-file.h"
#include "p2p-send-fileDlg.h"
#include "afxdialogex.h"
#include <sys/timeb.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int SendData(int fd, char* buff, int len) {
	int sended = 0;
	while (sended < len) {
		// 1 second timeout
		int wl = kkp2p_write(fd, buff + sended, len - sended, 1000);
		if (wl < 0) {
			printf("SendData error,fd:%d,ret:%d,len:%d,errno:%d,desc:%s.\n", fd, wl, len, errno, strerror(errno));
			return -1;
		}
		sended += wl;
	}
	return len;
}

int RecvData(int fd, char* buff, int len) {
	int recved = 0;
	while (recved < len) {
		// 1 second timeout
		int wl = kkp2p_read(fd, buff + recved, len - recved, 1000);
		if (wl < 0) {
			printf("RecvData error,fd:%d,ret:%d,len:%d,errno:%d,desc:%s.\n", fd, wl, len, errno, strerror(errno));
			return -1;
		}
		recved += wl;
	}
	return len;
}

bool IsHostLittleEndian() {
	static const int number = 1;
	return (1 == *(const char*)(&number));
}

uint64_t Ntohll(uint64_t val)
{
	if (IsHostLittleEndian()) {
		return (((uint64_t)ntohl((int32_t)((val << 32) >> 32))) << 32)
			| (uint32_t)ntohl((int32_t)(val >> 32));
	}
	else {
		return val;
	}
}

uint64_t Htonll(uint64_t val)
{
	if (IsHostLittleEndian()) {
		return (((uint64_t)htonl((int32_t)((val << 32) >> 32))) << 32)
			| (uint32_t)htonl((int32_t)(val >> 32));
	}
	else  {
		return val;
	}
}

uint64_t TimeMilliSecond()
{
	timeb now;
	ftime(&now);
	return now.time * 1000 + now.millitm;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);  

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cp2psendfileDlg
Cp2psendfileDlg::Cp2psendfileDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_P2PSENDFILE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cp2psendfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT9, m_loginDomain);
	DDX_Control(pDX, IDC_EDIT8, m_loginPort);
	DDX_Control(pDX, IDC_COMBO1, m_connectMode);
	DDX_Control(pDX, IDC_EDIT10, m_lanPort);
	DDX_Control(pDX, IDC_EDIT11, m_peerId);
	DDX_Control(pDX, IDC_EDIT6, m_connectType);
	DDX_Control(pDX, IDC_EDIT7, m_fileName);
	DDX_Control(pDX, IDC_EDIT12, m_statInfo);
	DDX_Control(pDX, IDC_BUTTON1, m_fileButton);
	DDX_Control(pDX, IDOK, m_startButton);
}

BEGIN_MESSAGE_MAP(Cp2psendfileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &Cp2psendfileDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &Cp2psendfileDlg::OnBnClickedOk)
END_MESSAGE_MAP()


void SetDropDownSize(CComboBox& box, UINT LinesToDisplay)
{
	ASSERT(IsWindow(box)); // Window must exist or SetWindowPos won't work 

	CRect cbSize; // current size of combo box 
	int Height; // new height for drop-down portion of combo box 

	box.GetClientRect(cbSize);
	Height = box.GetItemHeight(-1); // start with size of the edit-box portion 
	Height += box.GetItemHeight(0) * LinesToDisplay; // add height of lines of text 

	// Note: The use of SM_CYEDGE assumes that we're using Windows '95 
	// Now add on the height of the border of the edit box 
	Height += GetSystemMetrics(SM_CYEDGE) * 2; // top & bottom edges 

	// The height of the border of the drop-down box 
	Height += GetSystemMetrics(SM_CYEDGE) * 2; // top & bottom edges 

	// now set the size of the window 
	box.SetWindowPos(NULL, // not relative to any other windows 
		0, 0, // TopLeft corner doesn't change 
		cbSize.right, Height, // existing width, new height 
		SWP_NOMOVE | SWP_NOZORDER // don't move box or change z-ordering. 
	);
}

BOOL Cp2psendfileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			
	//SetIcon(m_hIcon, FALSE);
	m_loginDomain.SetWindowText(_T("112.74.167.65"));
	m_loginPort.SetWindowText(_T("4080"));
	m_lanPort.SetWindowText(_T("3549"));
	m_peerId.SetWindowText(_T("kkuai-ipc-00001"));
	m_connectType.EnableWindow(FALSE);
	m_statInfo.EnableWindow(FALSE);

	m_connectMode.AddString(_T("0|auto"));
	m_connectMode.AddString(_T("1|p2p"));
	m_connectMode.AddString(_T("2|relay"));
	m_connectMode.AddString(_T("lan search"));
	SetDropDownSize(m_connectMode, 5);
	return TRUE;
}

void Cp2psendfileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void Cp2psendfileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR Cp2psendfileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cp2psendfileDlg::OnBnClickedButton1()
{
	CFileDialog opendlg(TRUE, _T("*"), _T("*"), OFN_OVERWRITEPROMPT, _T("all files(*.*;)|*.*||"), NULL);
	if (opendlg.DoModal() == IDOK) {
		m_strFileName = opendlg.GetPathName();
	}

	m_fileName.SetWindowText(m_strFileName);

	UpdateData(TRUE);
}

UINT ThreadFun(LPVOID pParam)
{
	Cp2psendfileDlg*  p_Dlg = (Cp2psendfileDlg*)pParam;
	p_Dlg->BeginWork();
	return 0;
}

void Cp2psendfileDlg::OnBnClickedOk()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_loginDomain.GetWindowText(m_strDomain);

	CString serverPort;
	m_loginPort.GetWindowText(serverPort);
	char *buf = (LPSTR)(LPCTSTR)serverPort;
	m_usLoginPort = atoi(buf);

	m_lanPort.GetWindowText(serverPort);
	buf = (LPSTR)(LPCTSTR)serverPort;
	m_usLanPort = atoi(buf); 

	m_peerId.GetWindowText(m_strPeerId);

	m_loginDomain.EnableWindow(FALSE);
	m_loginPort.EnableWindow(FALSE);
	m_connectMode.EnableWindow(FALSE);
	m_lanPort.EnableWindow(FALSE);
	m_fileName.EnableWindow(FALSE);
	m_peerId.EnableWindow(FALSE);
	m_fileButton.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	::AfxBeginThread(ThreadFun, this);
	return;
}

void Cp2psendfileDlg::BeginWork()
{
	// open file
	char * szFileName = (LPSTR)(LPCTSTR)m_strFileName;
	if (szFileName == NULL)
	{
		AfxMessageBox(A2T("file name is empty"));
		return;
	}

	int nameLen = 0;
	char * startPos = strrchr(szFileName, '\\');
	if (startPos == NULL) {
		nameLen = strlen(szFileName);
		startPos = szFileName;
	} else {
		startPos++;
		nameLen = strlen(szFileName) - (startPos - szFileName);
	}

	FILE * pRead = fopen(szFileName, "rb");
	if (!pRead)
	{
		AfxMessageBox(A2T("open file error"));
		return;
	}

	// get file size
	uint64_t fileSize = 0;
	if (pRead)
	{
		_fseeki64(pRead, 0, SEEK_END);
		fileSize = _ftelli64(pRead);
		_fseeki64(pRead, 0, SEEK_SET);
	}
	
	memset(&m_engineConf, 0, sizeof(m_engineConf));
	m_engineConf.login_domain = m_strDomain.GetBuffer(0);
	m_engineConf.login_port = m_usLoginPort;
	m_engineConf.lan_search_port = m_usLanPort;

	kkp2p_engine_t* kkp2p_engine = kkp2p_engine_init(&m_engineConf, 10000);
	if (kkp2p_engine == NULL) {
		AfxMessageBox(A2T("kkp2p_engine_init error"));
		return;
	}

	// connect mode
	CString strMode;
	m_connectMode.GetWindowText(strMode);
	USES_CONVERSION;
	char* szMode= T2A(strMode.GetBuffer(0));

	kkp2p_connect_ctx_t ctx;
	memset(&ctx, 0, sizeof(kkp2p_connect_ctx_t));
	strncpy(ctx.peer_id, m_strPeerId.GetBuffer(0), 32);

	// creat alike tcp channel
	ctx.channel_type = KKP2P_TCP_CHANNEL;

	ctx.encrypt_data = 0;
	ctx.timeout = 3000;
	ctx.func = NULL;
	ctx.func_param = NULL;

	bool bLanSearch = false;
	if (strcmp(szMode, "0|auto") == 0) {
		ctx.connect_mode = 0;
	}else if (strcmp(szMode, "1|p2p") == 0) {
		ctx.connect_mode = 1;
	}else if (strcmp(szMode, "2|relay") == 0) {
		ctx.connect_mode = 2;
	} else if (strcmp(szMode, "lan search") == 0) {
		ctx.connect_mode = 1;
		bLanSearch = true;
	}

	int connRet = -1;
	kkp2p_channel_t channel;
	while (connRet != 0) {
		if (bLanSearch) {
			connRet = kkp2p_lan_search(kkp2p_engine, &ctx, &channel);
		}
		else {
			connRet = kkp2p_connect(kkp2p_engine, &ctx, &channel);
		}
	}

	// show connect type
	if (channel.transmit_mode == 1) {
		if (channel.is_ipv6_p2p) {
			m_connectType.SetWindowText(_T("p2p(ipv6)"));
		}
		else {
			m_connectType.SetWindowText(_T("p2p(ipv4)"));
		}
	} else if (channel.transmit_mode == 2) {
		m_connectType.SetWindowText(_T("relay"));
	}
	UpdateWindow();
	
	// protocol
	// fileSize(8 byte) + fileNameLen(2 byte) + fileName + fileBody
	uint64_t netFileSize = Htonll(fileSize);
	int ret = SendData(channel.fd, (char*)&netFileSize, sizeof(uint64_t));
	if (ret < 0) {
		AfxMessageBox(A2T("send file size error"));
		return;
	}

	uint16_t  netNameLen = htons(nameLen);
	ret = SendData(channel.fd, (char*)&netNameLen, sizeof(uint16_t));
	if (ret < 0) {
		AfxMessageBox(A2T("send name size error"));
		return;
	}

	ret = SendData(channel.fd, (char*)startPos, nameLen);
	if (ret < 0) {
		AfxMessageBox(A2T("send name size error"));
		return;
	}

	// loop send file body
	uint64_t startM = TimeMilliSecond();
	uint64_t endM = startM;
	char szBuff[1024];
	int readLen = fread(szBuff, 1, sizeof(szBuff), pRead);
	uint64_t totalSend = 0;
	uint32_t cnt = 0;
	while (readLen > 0) {
		ret = SendData(channel.fd, szBuff, readLen);
		if (ret < 0) {
			char info[1024];
			memset(info, 0, sizeof(info));
			sprintf(info,"send error,file len:%llu,send len:%llu,channel id:%u.\n", fileSize, totalSend, channel.channel_id);
			AfxMessageBox(A2T(info));

			kkp2p_close_fd(channel.fd);
			kkp2p_close_channel(kkp2p_engine, channel.channel_id);
			kkp2p_engine_destroy(kkp2p_engine);
			return ;
		}
		totalSend += ret;
		readLen = fread(szBuff, 1, sizeof(szBuff), pRead);

		if (++cnt % 100 == 0) {
			endM = TimeMilliSecond();
			if (endM > startM) {
				int speed = totalSend * 1000 / (endM - startM);
				char info[1024];
				sprintf(info, "file len:%lld,send len:%lld,speed:%d(byte/s)", fileSize, totalSend, speed);
				m_statInfo.SetWindowTextA(info);
				UpdateWindow();
			}
		}
		
	}
	fclose(pRead);

	endM = TimeMilliSecond();
	int speed = totalSend * 1000 / (endM - startM);
	char info[1024];
	sprintf(info, "file len:%lld,send len:%lld,send speed:%d(byte/s)", fileSize, totalSend, speed);
	m_statInfo.SetWindowTextA(info);
	UpdateWindow();

	// wait ack
	char ack;
	ret = RecvData(channel.fd, &ack, 1);
	if (ack == '1') {
		char info[1024];
		sprintf(info,"send finished,file len:%llu,send len:%llu", fileSize, totalSend);
		AfxMessageBox(A2T(info));
	}
	else {
		char info[1024];
		sprintf(info, "send error,file len:%llu,send len:%llu", fileSize, totalSend);
		AfxMessageBox(A2T(info));
	}

	kkp2p_close_fd(channel.fd);
	kkp2p_close_channel(kkp2p_engine, channel.channel_id);
	kkp2p_engine_destroy(kkp2p_engine);
}

