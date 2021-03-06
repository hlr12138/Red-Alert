﻿//
//  BuildingData.cpp
//  CCRA
//
//  Created by 王星洲 on 2018/5/23.
//  Modified by 王亮 on 2018/5/23
//

#include "Building.h"
#include "Scene/GameScene.h"
#include "cocos2d.h"


using namespace cocos2d;


//创建建筑物时调用的方法
Building* Building::create(Tag tag, int id, int iindex)
{
    //创建一个建筑物精灵对象
    Building* temp = new Building();
    //添加物理
    auto body = PhysicsBody::createCircle(temp -> getContentSize().width/2);
    
    body->setContactTestBitmask(1);//设置为1，开启其他刚体与此物理刚体产生碰撞，设置它才能触发碰撞检测回调函数
    
    temp->setPhysicsBody(body);

    //定义一个存放建筑物的字符串数组
    std::string picTable[6] = { "GameItem/Building/Base200.png","GameItem/Building/Power200.png","GameItem/Building/Mine200.png","GameItem/Building/Barracks150.png","GameItem/Building/CarFactory200.png","GameItem/Building/Satellite200.png" };
    //初始化安装建筑物时金钱的消耗
    int sValue[6] = { 500,500,500,500,500,300 };
	//电量消耗
	int sPower[6] = { 0,0,600,500,300,200 };
    //初始化建筑物的血量
    int sHP[6] = { 2000,500,500,500,500,500 };

	int index;
	if (tag != SATELLITE_TAG)
	{
		index = tag;
	}
	else
	{
		index = tag - 8;
	}
    
    
    //初始化建筑物精灵对象
    temp->initWithFile(picTable[index].c_str());
    //自动释放
    temp->autorelease();
    //拿到当前建筑物的_tag
    temp->_buildingTag = tag;
	//id
    temp->setID(id);
    //index
    temp->setIndex(iindex);
    //设置精灵自身Tag
    temp->setTag(tag);

    //设置血量
	temp->_FullHp = sHP[index];
	temp->_hp = temp->_FullHp;
    //根据_tag设置安装各建筑物时需要的金钱、电量
    temp->_value = sValue[index];
	temp->_castPower = sPower[index];
    //卖掉建筑物时得到的金钱
    temp->_sellValue = temp->_value / 2;

	//血槽
	temp->_bloodBox = Sprite::create("GameItem/BloodBar/BuildingBloodBox.png");
	temp->_bloodBox->setScale(temp->getContentSize().width / 200);
	temp->_bloodBox->setPosition(Vec2(temp->getContentSize().width / 2, temp->getContentSize().height + 10));
	temp->addChild(temp->_bloodBox);

	//血条
	temp->_bloodBarPt = ProgressTimer::create(Sprite::create("GameItem/BloodBar/BuildingBloodBar.png"));
	temp->_bloodBarPt->setScale(temp->getContentSize().width / 200); 
	temp->_bloodBarPt->setPosition(Vec2(temp->getContentSize().width / 2, temp->getContentSize().height + 10));
	temp->_bloodBarPt->setType(ProgressTimer::Type::BAR);
	temp->_bloodBarPt->setMidpoint(Vec2(0, 0.5));
	temp->_bloodBarPt->setPercentage(100);
	temp->addChild(temp->_bloodBarPt);

	//红色血条
	temp->_bloodBarAsEnemyPt = ProgressTimer::create(Sprite::create("GameItem/BloodBar/BuildingBloodBarEnemy.png"));
	temp->_bloodBarAsEnemyPt->setScale(temp->getContentSize().width / 200);
	temp->_bloodBarAsEnemyPt->setPosition(Vec2(temp->getContentSize().width / 2, temp->getContentSize().height + 10));
	temp->_bloodBarAsEnemyPt->setType(ProgressTimer::Type::BAR);
	temp->_bloodBarAsEnemyPt->setMidpoint(Vec2(0, 0.5));
	temp->_bloodBarAsEnemyPt->setPercentage(100);
	temp->_bloodBarAsEnemyPt->setVisible(false);    //默认不显示
	temp->addChild(temp->_bloodBarAsEnemyPt);
    
    return temp;
}


void Building::getInjuredBy(Unit * enemy)
{
	decreaseHP(enemy->getUnitATK());
}

void Building::decreaseHP(int num)
{
	_hp -= num;

	auto progressTo = ProgressTo::create(0.5f, 100 * _hp / _FullHp);
	if (_bloodBarPt->isVisible())
	{
		_bloodBarPt->runAction(progressTo);
	}
	else
	{
		_bloodBarAsEnemyPt->runAction(progressTo);
	}
}
