#include "MediaPlayer.h"
#include <string.h>
#include <vector>

void *lock(void *opaque, void **plane);

void  unlock(void *opaque, void *picture, void *const *planes);

void  display(void *opaque, void *picture);

unsigned video_format(void **opaque, char *chroma, unsigned *width, unsigned *height,
	unsigned *pitches, unsigned *lines);

void video_cleanup(void *opaque)
{

}

void HandlderCallBack(const VLCEvent *event, void *pUserData)
{
	MediaControl* m = (MediaControl*)pUserData;
}

MediaControl::MediaControl()
{
	m_EmStatus = S_PAUSE;

	m_nWidth = 3840;
	m_nHeight = 1920;
}


MediaControl::~MediaControl()
{
}

BOOL MediaControl::Init()
{
	const char * const vlc_args[] = {
		"-I", "dumy", "--plugin-path=./plugins" };
	const char* version = libvlc_get_version();

	m_pInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);

	m_pPlayer = libvlc_media_player_new(m_pInstance);

	m_pEventManager = libvlc_media_player_event_manager(m_pPlayer);
	//m_pInstance = libvlc_new(0, NULL);
	if (m_pInstance)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL MediaControl::UnInit()
{
	libvlc_media_release(m_pMedia);
	libvlc_release(m_pInstance);
	return FALSE;
}

BOOL MediaControl::Play()
{
	if (m_pPlayer)
	{
		if (m_EmStatus == S_PAUSE || m_EmStatus == S_STOP)
		{
			m_EmStatus = S_PLAY;
			libvlc_media_player_play(m_pPlayer);
		}
		else if (m_EmStatus == S_PLAY)
		{
			m_EmStatus = S_PAUSE;
			libvlc_media_player_pause(m_pPlayer);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL MediaControl::IsPlaying()
{
	if (m_pPlayer)
	{
		if (m_EmStatus == S_PLAY)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL MediaControl::Stop()
{
	if (m_pPlayer)
	{
		libvlc_media_player_stop(m_pPlayer);
		libvlc_media_release(m_pMedia);
		return TRUE;
	}
	return FALSE;
}

BOOL MediaControl::Seek(int64_t pos)
{
	if (m_pPlayer)
	{
		libvlc_time_t time = libvlc_media_player_get_time(m_pPlayer) + pos;
		int64_t allTime = libvlc_media_player_get_length(m_pPlayer);
		if (time > allTime)
		{
			time = allTime;
		}
		if (time < 0)
		{
			time = 0;
		}
		libvlc_media_player_set_time(m_pPlayer, time);
		return TRUE;
	}
	return FALSE;
}

BOOL MediaControl::Pause()
{
	if (m_pPlayer)
	{
		if (m_EmStatus == S_PLAY)
		{
			m_EmStatus = S_PAUSE;
			libvlc_media_player_pause(m_pPlayer);
		}
		return TRUE;
	}
	return FALSE;
}

//返回视屏的长度(ms)
int64_t MediaControl::GetLength()
{
	if (m_pPlayer)
	{
		int64_t allTime = libvlc_media_player_get_length(m_pPlayer);
		return allTime;
	}
	return 0;
}

int64_t MediaControl::GetCurrentPos()
{
	if (m_pPlayer)
	{
		int64_t time = libvlc_media_player_get_time(m_pPlayer);
		return time;
	}
	return 0;
}

BOOL MediaControl::NextFrame()
{
	if (m_pPlayer)
	{
		libvlc_media_player_next_frame(m_pPlayer);
		return TRUE;
	}
	return FALSE;
}

BOOL MediaControl::TakePhoto(const char* name, int width, int height)
{
	if (m_pPlayer)
	{
		if (width == 0 || height == 0)
		{
			width = m_pPraram->width;
			height = m_pPraram->height;
		}
		int ret = libvlc_video_take_snapshot(m_pPlayer, 0, name, width, height);

		return TRUE;
	}
	return FALSE;
}

BOOL MediaControl::LoadFile(const char* fileName)
{
	strcpy(m_strFileName, fileName);
	std::string strPath(fileName);
	bool bURL = false;
	std::vector<std::string> vctURL;

	vctURL.push_back("http");
	vctURL.push_back("https");
	vctURL.push_back("ftp");
	vctURL.push_back("rtmp");

	for (unsigned i = 0; i < vctURL.size(); i++)
	{
		// 实际使用请判断大小写
		if (!strPath.compare(0, vctURL[i].size(), vctURL[i]))
		{
			bURL = true;
			break;
		}
	}
	if (bURL)
	{
		m_pMedia = libvlc_media_new_location(m_pInstance, fileName); //network media stream
																	 //libvlc_media_add_option(m_pMedia, ":network-caching=100");
	}
	else
	{
		m_pMedia = libvlc_media_new_path(m_pInstance, fileName); //local media
	}
	libvlc_media_player_set_media(m_pPlayer, m_pMedia);
	libvlc_media_add_option(m_pMedia, ":avcodec-hw=none");
	//libvlc_media_add_option(m_pMedia, ":h264-fps=60");
	m_pEventManager = libvlc_media_player_event_manager(m_pPlayer);
	//libvlc_media_player_set_hwnd(m_pPlayer,this->m_hWnd);
	m_pPraram = new TCallbackParam();
	m_pPraram->pThis = m_pSceneView;

	libvlc_video_set_callbacks(m_pPlayer, lock, unlock, display, m_pPraram);
	//libvlc_video_set_format(m_pPlayer, "YUYV", m_nWidth, m_nHeight, m_nWidth * 2);
	libvlc_video_set_format_callbacks(m_pPlayer, video_format, NULL);

	Play();

	return TRUE;
}

float MediaControl::GetVideoFps()
{
	if (m_pPlayer)
	{
		return libvlc_media_player_get_fps(m_pPlayer);
	}
	return 0.0f;
}

libvlc_media_track_info_t* MediaControl::GetTrackInfo()
{
	if (m_pPlayer)
	{
		libvlc_media_track_info_t* info = new libvlc_media_track_info_t;
		libvlc_media_get_tracks_info(m_pMedia, &info);
		return info;
	}
	return NULL;
}

void MediaControl::SetEventHandler(VLCEventHandler event, void* pUserData)
{
	eventHandler = event;
	libvlc_event_attach(m_pEventManager, libvlc_MediaPlayerTimeChanged, eventHandler, pUserData);
}
