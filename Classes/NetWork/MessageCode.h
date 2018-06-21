/*****************************************************************************
*  Copyright (C) 2017 李坤 1061152718@qq.com
*
*  此文件属于软件学院2017c++大项目泡泡堂选题的项目文件.
*
*  此项目是开源项目, 在期末答辩之后, 我们可能会在假期里对一些因时间不够未完成的功能进
*  行补充, 以及适配windows平台, 在未来如果技术允许的情况下, 会酌情开发ios版本和anroid
*  版本, 期待您能够为这个开源项目提供宝贵意见, 帮助我们做得更好, 如果能够贡献宝贵的代
*  码那就更令人开心了.
*
*  本项目遵守MIT开源协议, 这也就是说, 您需要遵守的唯一条件就是在修改后的代码或者发行
*  包包含原作者的许可信息. 除非获得原作者的特殊许可, 在任何项目(包括商业项目)里使用
*  本项目的文件都需要包含作者的许可.
*
*  如果对项目有疑问或者其他建议, 欢迎联系13167211978@163.com, 1061152718@qq.com,
*  我们期待能和您互相交流合作, 学习更多的知识.
*
*  另外注意: 此项目需要您自行配置cocos环境,安装boost库, 如果遇到相关问题的话, 欢迎将
*  错误日志发给我们, 您的帮助将有助于改善游戏的体验.
*
*  @file     MessageCode.h
*  @brief    定义传输命令的编码
*  
*
*  @author   李坤
*  @email    1061152718@qq.com
*  @version  4.0.1.5(版本号)
*  @date     2017/06/11
*  @license  Massachusetts Institute of Technology License (MIT)
*
*----------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2017/06/11 | 4.0.0.1   | 李坤            | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef MessageCode_h
#define MessageCode_h

/**
            * @name 询问网络中是否存在已经建立好的游戏房间
            * @{
            */
            #define QUERY_FOR_ROOM  "a"
       		/** @} */

/**
            * @name 回应QUERY_FOR_ROOM, 并提供房间基本信息
            * @{
            */
            #define ANSWER_FOR_ROOM "b"
       		/** @} */

/**
            * @name 开启client的filter mode, 并传输过滤关键词
            * @{
            */
            #define KEY_ORDER       "c"
       		/** @} */

/**
            * @name 广播开始游戏指令
            * @{
            */
            #define START_GAME      "d"
       		/** @} */

/**
            * @name 向房间内所有玩家传输新加入房间的玩家信息
            * @{
            */
            #define JOIN_ROOM       "e"
       		/** @} */

/**
            * @name 新加入房间的玩家向房间拥有者请求房间内所有玩家的信息
            * @{
            */
            #define QUERY_FOR_PLAYERS_IN_ROOM "f"
       		/** @} */

/**
            * @name QUERY_FOR_PLAYERS_IN_ROOM, 房间所有者\n
            * 		向新加入者传输房间内所有玩家的信息
            * @{
            */
            #define ANSWER_FOR_PLAYERS_IN_ROOM "g"
       		/** @} */

/**
            * @name 向房间内所有玩家传输聊天信息
            * @{
            */
            #define CHAT_MESSAGE    "h"
       		/** @} */

/**
            * @name 向房间内其他玩家传输房主所选择的地图索引
            * @{
            */
            #define MAP_SELECT      "i"
       		/** @} */

/**
            * @name 向游戏内其他玩家广播死亡消息，确保同步死亡
            * @{
            */
            #define DEAD_MESSAGE    "j"
       		/** @} */

/**
			* @name 在游戏内广播Unit移动消息
			* @{
			*/
			#define MOVE_UNIT    "k"
			/** @} */

/**
			* @name 在游戏内广播建造建筑消息
			* @{
			*/
			#define CREATE_BUILDING    "l"
			/** @} */

/**
			* @name 在游戏内广播创建Unit消息
			* @{
			*/
			#define CREATE_UNIT    "m"
			/** @} */

/**
			* @name 在游戏内广播移除建筑消息
			* @{
			*/
			#define REMOVE_BUILDING    "n"
			/** @} */

/**
			* @name 在游戏内广播移除Unit消息
			* @{
			*/
			#define REMOVE_UNIT    "o"
			/** @} */




#endif /* MessageCode_h */
