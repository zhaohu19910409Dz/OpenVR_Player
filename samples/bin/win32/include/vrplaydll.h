#ifndef VRPLAYDLL_H
#define VRPLAYDLL_H

#if defined(VRPLAYDLL_LIBRARY)
#  define VRPLAYDLLSHARED_EXPORT __declspec(dllexport)
#else
#  define VRPLAYDLLSHARED_EXPORT __declspec(dllimport)
#endif

/**
 * 说明：定义显示回调函数
 * w - 图像的宽度
 * h - 图像的高度
 * data      - 指向图像数据的指针数组
 * sumSize   - 图像数据的总大小，便于new时分配空间
 * plansSize - planeSize[0] = Y, planeSize[1] = U, planeSize[2] = V
 * pUser     - 传递init中pUser定义的指针
 * 说明：
 * 数据指针访问：Y = data, U = data + planeSize[0], V = data + planeSize[0] + planeSize[1]
 */
typedef void (*DisplayCallback)(int w, int h, void * data, int sumSize, int * planeSize, void * pUser);

// 这里的定义是便于动态加载
typedef bool (* DllFuncInit)(DisplayCallback func, void * pUser);
typedef void (* DllFuncStart)(void);
typedef void (* DllFuncStop)(void);

extern "C"
{
    /**
     * @brief 初始化
     * func  - 定义回调的函数
     * pUser - 用于给回调函数传递额外的信息，如果不需要刻意设置为NULL
     * @return
     */
    VRPLAYDLLSHARED_EXPORT bool init(DisplayCallback func, void * pUser);

    /**
     * @brief 开始播放
     */
    VRPLAYDLLSHARED_EXPORT void start();

    /**
     * @brief 停止播放
     */
    VRPLAYDLLSHARED_EXPORT void stop();
}

#endif // VRPLAYDLL_H
