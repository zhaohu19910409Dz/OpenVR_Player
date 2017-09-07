#pragma once
#ifndef __MediaControl_H__
#define	__MediaControl_H__

#include <Windows.h>
#include "tchar.h"
#include "vlc/vlc.h"
#include "vlc/libvlc_events.h"

#pragma  comment(lib,"libvlc.lib")
#pragma  comment(lib,"libvlccore.lib")

#include <queue>
typedef struct IndexQueue
{
	std::queue<int> mIndex;
	CRITICAL_SECTION cs;

	IndexQueue()
	{
		::InitializeCriticalSection(&cs);
		while (!mIndex.empty())
		{
			mIndex.pop();
		}
	}

	~IndexQueue()
	{
		::DeleteCriticalSection(&cs);
	}

	int GetSize()
	{
		return mIndex.size();
	}

	void Pop()
	{
		::EnterCriticalSection(&cs);
		if (mIndex.size() > 0)
		{
			mIndex.pop();
		}
		::LeaveCriticalSection(&cs);
	}

	int GetFront()
	{
		if (mIndex.size() > 0)
		{
			int ret = mIndex.front();
			return ret;
		}
		else 
		{
			return -1;
		}		
	}

	void Push(int index)
	{
		::EnterCriticalSection(&cs);
		mIndex.push(index);
		::LeaveCriticalSection(&cs);
	}
};

typedef struct TCallbackParam
{
	int		pitch;
	int		width;
	int		height;
	int		index;
	HWND	hWnd;
	unsigned char			*pixels;
	void	*pThis;
	TCallbackParam()
	{

	}

	~TCallbackParam()
	{

	}
}TCallbackParam;

enum PlayerStatus
{
	S_UNKOWN = -1,
	S_PLAY,
	S_PAUSE,
	S_STOP
};

inline void Unicode2UFT8(TCHAR* src, char* des)
{
	int length = ::WideCharToMultiByte(CP_UTF8, NULL, src, _tcslen(src), NULL, 0, NULL, NULL);
	char* buf = new char[length + 1];
	::WideCharToMultiByte(CP_UTF8, NULL, src, _tcslen(src), buf, length, NULL, NULL);
	buf[length] = '\0';
	strcpy(des, buf);
	delete[] buf;
}

struct libvlc_instance_t;
struct libvlc_media_player_t;
struct libvlc_media_t;
struct libvlc_event_manager_t;

// Typedefs for VLC types
typedef struct libvlc_event_t           VLCEvent;                     ///< A vlc event.
typedef void(*VLCEventHandler)         (const VLCEvent *, void *);   ///< Event handler callback.

class MediaControl
{
public:
	MediaControl();
	~MediaControl();
	//
	BOOL Init();
	BOOL UnInit();
	//
	BOOL Play();
	BOOL IsPlaying();
	BOOL Stop();
	BOOL Seek(int64_t pos);
	BOOL Pause();
	int64_t GetLength();
	int64_t GetCurrentPos();
	BOOL NextFrame();
	BOOL TakePhoto(const char* name, int width = 0, int height = 0);
	//
	BOOL LoadFile(const char* fileName);
	float GetVideoFps();
	libvlc_media_track_info_t* GetTrackInfo();
	void SetEventHandler(VLCEventHandler event, void* pUserData);
public:
	libvlc_instance_t		*m_pInstance;
	libvlc_media_player_t	*m_pPlayer;
	libvlc_media_t			*m_pMedia;
	libvlc_time_t			*m_pTime;

	libvlc_event_manager_t*  m_pEventManager;       ///< The event manger for the loaded media file.    
	VLCEventHandler          eventHandler;         ///< An event handler for the media player.   
	void *                   pHwnd_;

	int						m_nWidth;
	int						m_nHeight;
	int						m_nLength;

	PlayerStatus			m_EmStatus;
	TCallbackParam			*m_pPraram;
public:
	HWND					m_hWnd;
	float					m_lFps;
	void*					m_pSceneView;
	char					m_strFileName[256];
};

#endif //_MediaControl_H__
