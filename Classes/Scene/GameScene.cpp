﻿#include "Scene/WelcomeScene.h"  
#include "Scene/GameScene.h"  
#include "ui/CocosGUI.h"
#include "Panel/Panel.h"
#include "Scene/EndingScene.h"

#define small_mapX 300
#define small_mapY 300
#define MINLENTH 15
#define SPEED 20


static int _mapIndex = 1;
static std::string splayerName;
static Client* clients;
//a static pointer which is gong to be used to make LevelData oject reference count nonzero
static LevelData* ptr = NULL;
static Panel* ptr_panel = NULL;

USING_NS_CC;
using namespace ui;

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in WelcomeScene.cpp\n");
}

Scene* GameScene::createScene(LevelData &data, Client* client, std::string playerName)
{
	auto scene = Scene::createWithPhysics();

	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));

	data.retain();

	_mapIndex = data.getmapIndex();
	clients = client;
	splayerName = playerName;
	//Make LevelData oject reference count nonzero
	ptr = &data;

	auto layer = GameScene::create();

	scene->addChild(layer);

	return scene;

}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	hasFog = true;
	memset(fog, 0, sizeof(fog));
	//大黑快快
	this->addChild(drawNode,3);
	//绿点点
	this->addChild(drawNode2, 6);
	//黑快快
	this->addChild(drawNode3, 7);
	//白边边
	this->addChild(drawNode4, 8);
	_thisScene = this;
	_client = clients;
	_inputData = ptr;
	_localPlayerName = splayerName;
	_playerList = ptr->player_list;
    for (auto& playerData : _playerList)
    {
        if (_localPlayerName == playerData.player_name)
        {
            _localPlayerID = playerData.player_id;
        }
    }


	//_enemySoldiers.pushBack(Unit::create(BASE_CAR_TAG));
    _unitIndex = 0;
    std::memset(_unitIndexDied, 0, 5000 * sizeof(int));
    _buildingIndex = 0;
    std::memset(_buildingIndexDied, 0, 5000 * sizeof(int));

    auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//===================Load map=========================
	if (_inputData->getmapIndex() == 1)
	{
		_tileMap = TMXTiledMap::create("GameItem/Map/mapbeautiful1.tmx");
	}
	else if (_inputData->getmapIndex() == 2)
	{
		_tileMap = TMXTiledMap::create("GameItem/Map/mapbeautiful2.tmx");
	}
	
    MAPX = _tileMap->getMapSize().width * _tileMap->getTileSize().width;
    MAPY = _tileMap->getMapSize().height * _tileMap->getTileSize().height;
    switch (_localPlayerID)
    {
    case 1:
        _tileMap->setPosition(Vec2::ZERO);
        break;
    case 2:
        _tileMap->setPosition(Vec2(visibleSize.width - MAPX, visibleSize.height - MAPY));
        break;
    case 3:
        _tileMap->setPosition(Vec2(0, visibleSize.height - MAPY));
        break;
    case 4:
        _tileMap->setPosition(Vec2(visibleSize.width - MAPX, 0));
        break;
    }
	this->addChild(_tileMap);

	_barrier = _tileMap->getLayer("barrier");



	/*update by czd*/
	if (_inputData->getmapIndex() == 1)
	{
		small_map = Sprite::create("GameItem/Map/small_map1.png");
	}
	else if (_inputData->getmapIndex() == 2)
	{
		small_map = Sprite::create("GameItem/Map/small_map2.png");
	}
    small_map->setPosition(Point(visibleSize.width - small_mapX / 2, visibleSize.height - small_mapX / 2));
	Vec2 p1[4];
	p1[0] = Vec2(visibleSize.width - small_mapX-1, visibleSize.height - small_mapY-1);
	p1[1] = Vec2(visibleSize.width - small_mapX , visibleSize.height - small_mapY-1 );
	p1[2] = Vec2(visibleSize.width - small_mapX, visibleSize.height);
	p1[3] = Vec2(visibleSize.width - small_mapX-1, visibleSize.height);
	drawNode4->drawPolygon(p1, 4, Color4F(1, 1, 1, 1), 1, Color4F(1, 1, 1, 1));

	Vec2 p2[4];
	p2[0] = Vec2(visibleSize.width - small_mapX - 1, visibleSize.height - small_mapY - 1);
	p2[1] = Vec2(visibleSize.width, visibleSize.height - small_mapY - 1);
	p2[2] = Vec2(visibleSize.width, visibleSize.height - small_mapY);
	p2[3] = Vec2(visibleSize.width - small_mapX - 1, visibleSize.height - small_mapY);
	drawNode4->drawPolygon(p2, 4, Color4F(1, 1, 1, 1), 1, Color4F(1, 1, 1, 1));



	this->addChild(small_map, 5);


    //DrawNode
    DrawNode* drawNode = DrawNode::create();
    this->addChild(drawNode);

	//=====================添加Panel========================
	auto panel = Panel::createWithGameScene(this);
	if (panel == nullptr) { log("create panel error"); }
	//auto _panelSize = panel->getContentSize();   //为什么是0，0？
	//log("%f %f %f %f",_panelSize.width,_panelSize.height,panel->getAnchorPoint().x,panel->getAnchorPoint().y);
	panel->setPosition(visibleSize.width - 112, visibleSize.height - 400);
	this->addChild(panel, 5);
	ptr_panel = panel;
	//log("the tag of panel is:%d", panel->getTag());

//===============================监听地图精灵=====================================
	_gameListener = EventListenerTouchOneByOne::create();
	_gameListener->onTouchBegan = [=](Touch* touch, Event* event) {
		//=========== 点击小地图的移动功能 ===============
		Point position = touch->getLocation();
		if (position.x > visibleSize.width - small_mapX && position.y > visibleSize.height - small_mapY) 
        {
            auto X = (position.x - (visibleSize.width - small_mapX)) / small_mapX * MAPX - visibleSize.width / 2;
            auto Y = (position.y - (visibleSize.height - small_mapY)) / small_mapY * MAPY - visibleSize.width / 2;
			if (X < 0) X = 0;
			if (Y < 0) Y = 0;
            if (X > MAPX - visibleSize.width) 
                X = MAPX - visibleSize.width;
            if (Y > MAPY - visibleSize.height) 
                Y = MAPY - visibleSize.height;
            //direction to move sprites
            Vec2 direction = Point(-X, -Y) - _tileMap->getPosition();

			_tileMap->setPosition(Point(-X, -Y));
            moveSpritesWithMap(direction);

            // if sell menu exit, remove it
            if (_isSellMenuExit)
            {
                removeChild(_sellBuildingMenu);
                _sellBuilding = nullptr;
                _isSellMenuExit = false;
            }
            
			return false;
		}
        else if (position.x > visibleSize.width - 224 && 
            position.y < 444 && position.y > 356)                //click the menus
        {
            return false;
        }
		else
		{
			_touchBegan = position;   // 记录起点
		}
        // if sell menu exit, remove it
        if (_isSellMenuExit)
        {
            removeChild(_sellBuildingMenu);
            _sellBuilding = nullptr;
            _isSellMenuExit = false;
        }

		return true;
	};

    // 选择时画出矩形
    _gameListener->onTouchMoved = [=](Touch* touch, Event* event) {
        Point movePosition = touch->getLocation();
        drawNode->clear();
        drawNode->drawRect(_touchBegan, Vec2(_touchBegan.x, movePosition.y), 
            movePosition, Vec2(movePosition.x, _touchBegan.y), Color4F(255, 255, 255, 100));
    };

	_gameListener->onTouchEnded = [=](Touch* touch, Event* event) {
	    _touchEnd = touch->getLocation();
        drawNode->clear();
        if (fabs(_touchEnd.x - _touchBegan.x) < 15.0 && 
            fabs(_touchEnd.y - _touchBegan.y) < 15.0)      // 点击则判断点击对象
        {
            //生成Sprite的Rect
            auto target = static_cast<Sprite*>(event->getCurrentTarget());
            Vec2 locationInTarget = target->convertToNodeSpace(touch->getLocation());
            Size size = target->getContentSize();
            Rect rect(0, 0, size.width, size.height);

            if (rect.containsPoint(locationInTarget))
            {
                switch (target->getTag())
                {
                case INFANTRY_TAG:
                case DOG_TAG:
                case TANK_TAG:
                    if (_soldiers.contains(static_cast<Unit*>(target)))
                    {
						for (Unit* unit : _selectedSoldiers)
						{
							unit->setIsSelected(false);
						}
                        _selectedSoldiers.clear();
                        _selectedSoldiers.pushBack((static_cast<Unit*>(target)));
						(static_cast<Unit*>(target))->setIsSelected(true);
                        break;
                    }
                    // if click enemy
                    _manager->setEnemy(static_cast<Unit*>(target));
                    break;
                case POWER_PLANT_TAG:
                case MINE_TAG:
                case CAR_FACTORY_TAG:
                case BASE_TAG:
                case BARRACKS_TAG:
				case SATELLITE_TAG:
				case DEFENSE_BUILDING_TAG:
                    for (auto& building : _buildings)
                    {
                        if (building == target)
                        {
                            if (!inDiamond(building->getPosition(), size.width/2, size.height/2, _touchEnd))
                            {
                                return;
                            }
                            //因为即使菱形也有点误差,所以决定,不能生成两个sell.
                            if (_isSellMenuExit)
                            {
                                return;
                            }
                            _sellBuilding = building;
                            //sell menu
                            auto sellBuildingMenuItem = MenuItemImage::create("Scene/sell_up.png", "Scene/sell_down.png",
                                CC_CALLBACK_0(GameScene::sellBuildingCallBack, this));
                            sellBuildingMenuItem->setPosition(building->getPosition());
                            _sellBuildingMenu = Menu::create(sellBuildingMenuItem, NULL);
                            _sellBuildingMenu->setPosition(Vec2::ZERO);
                            addChild(_sellBuildingMenu, 4);
                            _isSellMenuExit = true;

                            return;
                        }
                    }
                    _manager->setBuilding(static_cast<Building*>(target));
                    return;
				case BASE_CAR_TAG:       //if there is any definition in the case
                {                        // you must use {} to contain it
					if (_soldiers.contains(static_cast<Unit*>(target)))
					{
						if (!static_cast<Unit*>(target)->getIsSelected())    //第一次单击选中基地车，第二次单击展开
						{
							for (Unit* unit : _selectedSoldiers)
							{
								unit->setIsSelected(false);
							}
							_selectedSoldiers.clear();
							_selectedSoldiers.pushBack((static_cast<Unit*>(target)));
							(static_cast<Unit*>(target))->setIsSelected(true);
							break;
						}
						else
						{
							//基地车展开成基地
							//移除基地车
							for (Unit* unit : _selectedSoldiers)
							{
								unit->setIsSelected(false);
							}
							_selectedSoldiers.clear();
							Vec2 position = target->getPosition();
							_client->sendMessage(REMOVE_UNIT, _manager->getRemoveUnitMessage(static_cast<Unit*>(target)));
							_soldiers.eraseObject(static_cast<Unit*>(target), false);
                            _unitIndexDied[static_cast<Unit*>(target)->getIndex()] = 1;
							this->removeChild(target);
							//创建基地
							Building* base = Building::create(BASE_TAG, _localPlayerID, _buildingIndex);
                            ++_buildingIndex;
							_gameEventDispatcher->addEventListenerWithSceneGraphPriority
							(_gameListener->clone(), base);
							base->setPosition(position);
							this->addChild(base, 2);
							_isBaseExist = true;
							_buildings.pushBack(base);
							_client->sendMessage(CREATE_BUILDING, _manager->getCreateBuildingMessage(position, BASE_TAG));
							//刷新Panel
							panel->setCurButton(panel->getCurCategoryTag());
							break;
						}
					}
					_manager->setEnemy(static_cast<Unit*>(target));
					break;
					
                }

                default:
                    // 为层注册监听器后层也会响应 所以此处需要判断士兵建筑和空地
                    //log("default");
                    if (_manager->getMoveController()->canPut(_touchEnd))
                    {
                        log("can put");
                        _manager->getMoveController()->setDestination(_touchEnd);
                    }
                    break;

                }
            }
        }
		else                              // 矩形框选择士兵
		{
			_manager->getMoveController()->selectSoldiersWithMouse(_touchBegan, _touchEnd);
		}
    };

	_gameEventDispatcher = Director::getInstance()->getEventDispatcher();
	_gameEventDispatcher->addEventListenerWithSceneGraphPriority(_gameListener, this);

//============================================键盘监听 by czd======================================
    auto _keyboardListener = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
            _keyUp = true;
            //CCLOG("按下了：上方向键");
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
            _keyLeft = true;
            //CCLOG("按下了：左方向键");
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
            _keyRight = true;
            //CCLOG("按下了：右方向键");
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
            _keyDown = true;
            //CCLOG("按下了：下方向键");
        }

        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);
    auto _keyboardReleasedListener = EventListenerKeyboard::create();
    _keyboardReleasedListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
            _keyUp = false;
            //CCLOG("松开了：上方向键");
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
            _keyLeft = false;
            //CCLOG("按下了：左方向键");
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
            _keyRight = false;
            //CCLOG("按下了：右方向键");
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
            _keyDown = false;
            //CCLOG("松开了：下方向键");
        }

        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardReleasedListener, this);

//==============================鼠标移动地图 by czd============================================
	auto _mouseOutBoradListener = EventListenerMouse::create();
	_mouseOutBoradListener->onMouseMove = [&](Event* event) {
		EventMouse* pem = static_cast<EventMouse*>(event);
		_cursorPosition = Vec2(pem->getCursorX(), pem->getCursorY());
	};
    //right button down
    _mouseOutBoradListener->onMouseDown = [&](Event* event) {
        EventMouse* eventMouse = static_cast<EventMouse*>(event);
        auto position = eventMouse->getLocationInView();
        auto mouseButton = eventMouse->getMouseButton();
        if (static_cast<int>(mouseButton))     // mouse right button = 1
        {
            for (auto& soldier : _soldiers)
            {
                Rect rect = Rect(soldier->getPositionX() - soldier->getContentSize().width / 2,
                    soldier->getPositionY() - soldier->getContentSize().height / 2,
                    soldier->getContentSize().width, soldier->getContentSize().height);
                log("%f %f %f %f", rect.getMinX(), rect.getMinY(), rect.size.width, rect.size.height);
                log("position %f %f", position.x, position.y);
                if (rect.containsPoint(position))
                {
                    _manager->setEnemy(soldier);
                    return;
                }
            }
            for (auto& building : _buildings)
            {
                Rect rect = Rect(building->getPositionX() - building->getContentSize().width / 2,
                    building->getPositionY() - building->getContentSize().height / 2,
                    building->getContentSize().width, building->getContentSize().height);
                if (rect.containsPoint(position))
                {
                    return;
                }
            }
			for (Unit* unit : _selectedSoldiers)
			{
				unit->setIsSelected(false);
			}
            _selectedSoldiers.clear();
        }
    };
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_mouseOutBoradListener, 1);

//================================back menu============================================

	auto backItem = MenuItemImage::create(
		"backNormal.png",
		"backSelected.png",
		CC_CALLBACK_1(GameScene::menuBackCallback, this));

	if (backItem == nullptr ||
		backItem->getContentSize().width <= 0 ||
		backItem->getContentSize().height <= 0)
	{
		problemLoading("'backNormal.png' and 'backSelected.png'");
	}
	else
	{
		float x = origin.x + backItem->getContentSize().width / 2;
		float y = origin.y + visibleSize.height - backItem->getContentSize().height / 2;
		backItem->setPosition(Vec2(x, y));
	}

	// create menu, it's an autorelease object
	auto menu = Menu::create(backItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 4);

//===================添加基地车==========================
	auto baseCar = Unit::create(BASE_CAR_TAG, _localPlayerID, _unitIndex);
    ++_unitIndex;
	baseCar->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    baseCar->setDestination(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	baseCar->setGetDestination(true);
	this->addChild(baseCar, 1);
	_gameEventDispatcher->addEventListenerWithSceneGraphPriority
	(_gameListener->clone(), baseCar);
	_soldiers.pushBack(baseCar);
	//log("%f %f", baseCar->getPosition().x, baseCar->getPosition().y);

	this->dataInit();




//===============================the first show of money and time===========================
    // 显示金钱
    //金币
    Sprite* coin = Sprite::create("Scene/coin.png");
    coin->setPosition(50, 50);
    addChild(coin, 3);
    //钱数
    sprintf(_moneyStr, "%d", _money);
    _moneyCount = Label::createWithTTF(_moneyStr, "fonts/Marker Felt.ttf", 36);
    _moneyCount->setPosition(120, 50);
    _moneyCount->setColor(Color3B(255, 185, 15)); //颜色
    addChild(_moneyCount, 3);

    // 显示时间
    // 时钟
    Sprite* clock = Sprite::create("Scene/clock.png");
    clock->setPosition(55, 100);
    addChild(clock, 3);
    // 时间
    sprintf(_timeStr, "%d:%d:%d", _time / 3600, _time / 60, _time % 60);
    _timeCount = Label::createWithTTF(_timeStr, "fonts/Marker Felt.ttf", 28);
    _timeCount->setPosition(120, 100);
    _timeCount->setColor(Color3B(0, 0, 0)); //颜色
    addChild(_timeCount, 3);

	scheduleUpdate();
    schedule(schedule_selector(GameScene::printTime), 1.0f); // 每隔1s执行一次

//=========================================power bar=======================================
    Sprite* powerBarBg = Sprite::create("Scene/PowerBg.png");
    powerBarBg->setPosition(50, 450);
    addChild(powerBarBg, 3);

    Sprite* powerBar = Sprite::create("Scene/Power.png");
    _powerBar = ProgressTimer::create(powerBar);
    _powerBar->setType(ProgressTimerType::BAR);
    _powerBar->setMidpoint(Point(0, 1));
    _powerBar->setBarChangeRate(Point(0, 1));
    _powerBar->setPercentage(0.0);
    _powerBar->setPosition(50, 450);
    addChild(_powerBar, 4);

	_manager = Manager::createWithGameScene(this);

//==================================================some retain====================================
	//由于GameScene中对panel指针的内存管理存在暂时无法解决的问题，所以采用直接向manager传递指针的方式
	_manager->setPanel(panel);

	_manager->retain();
	_manager->getMoveController()->retain();

	panel->retain();
	panel->_powerPlantIcon->retain();
	panel->_mineIcon->retain();
	panel->_barracksIcon->retain();
	panel->_carFactoryIcon->retain();
	panel->_infantryIcon->retain();
	panel->_dogIcon->retain();
	panel->_tankIcon->retain();

	return true;
}

void GameScene::onEnter()
{
    Layer::onEnter();
    auto listener = EventListenerPhysicsContact::create();
    listener->onContactBegin = [](PhysicsContact & contact)
    {
        //        auto spriteA = (Sprite *)contact.getShapeA() -> getBody() -> getNode();
        //        auto spriteB = (Sprite *)contact.getShapeB() -> getBody() -> getNode();
        log("onContact");
        return true;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);
}

//void GameScene::onExitTransitionDidStart()
//{
//    Layer::onExitTransitionDidStart();
//    _gameEventDispatcher->removeAllEventListeners();
//    //释放定时器
//    this->unscheduleUpdate();
//    this->unscheduleAllSelectors();
//}

void GameScene::dataInit()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    _cursorPosition = 0.5 * Director::getInstance()->getVisibleSize();

	_isPowerEnough = false;
	_money = 2000;
	_power = 0;

	_barracksNum = 0;
	_mineNum = 0;
	_powerPlantNum = 0;
	_carFactoryNum = 0;
	_satelliteNum = 0;

    _tankNum = 0;
    _infantryNum = 0;
    _dogNum = 0;

	_carFactoryPosition = _barracksPosition = Vec2::ZERO;

    //init base car
    float right = MAPX - visibleSize.width / 2;
    float left = visibleSize.width / 2 - MAPX + visibleSize.width;
    float up = MAPY - visibleSize.height / 2;
    float down = visibleSize.height / 2 - MAPY + visibleSize.height;
    for (auto& player : _playerList)
    {
        if (player.player_id == _localPlayerID)
        {
            continue;
        }
        auto baseCar = Unit::create(BASE_CAR_TAG, player.player_id, _unitIndex);
        ++_unitIndex;
        switch (_localPlayerID)
        {
        case 1:
            switch (player.player_id)
            {
            case 2:
                baseCar->setPosition(Vec2(right, up));
                break;
            case 3:
                baseCar->setPosition(Vec2(visibleSize.width / 2, up));
                break;
            case 4:
                baseCar->setPosition(Vec2(right, visibleSize.height / 2));
                break;
            }
            break;
        case 2:
            switch (player.player_id)
            {
            case 1:
                baseCar->setPosition(Vec2(left, down));
                break;
            case 3:
                baseCar->setPosition(Vec2(left, visibleSize.height));
                break;
            case 4:
                baseCar->setPosition(Vec2(visibleSize.width, down));
                break;
            }
            break;
        case 3:
            switch (player.player_id)
            {
            case 1:
                baseCar->setPosition(Vec2(visibleSize.width / 2, down));
                break;
            case 2:
                baseCar->setPosition(Vec2(right, visibleSize.height / 2));
                break;
            case 4:
                baseCar->setPosition(Vec2(right, down));
                break;
            }
            break;
        case 4:
            switch (player.player_id)
            {
            case 1:
                baseCar->setPosition(Vec2(left, visibleSize.height / 2));
                break;
            case 2:
                baseCar->setPosition(Vec2(visibleSize.width / 2, up));
                break;
            case 3:
                baseCar->setPosition(Vec2(left, up));
                break;
            }
            break;
        }
        
        baseCar->setGetDestination(true);
        baseCar->_bloodBarPt->setVisible(false);
        baseCar->_bloodBarAsEnemyPt->setVisible(true);
        this->addChild(baseCar, 1);
        _gameEventDispatcher->addEventListenerWithSceneGraphPriority
        (_gameListener->clone(), baseCar);
        pushEnemyUnitByID(baseCar, player.player_id);

    }

	_isBaseExist = false;
}

void GameScene::menuBackCallback(Ref *pSender)
{
	//跳转到第一个场景，记得包含第一个场景的头文件：GameScene.h  
	//Director::getInstance()->replaceScene(MyFirstScene::createScene());  
	Director::getInstance()->popScene();
}

void GameScene::pushEnemyUnitByID(Unit* u, int id)
{
    switch (id)
    {
    case 1:
        _enemySoldiers1.pushBack(u);
        break;
    case 2:
        _enemySoldiers2.pushBack(u);
        break;
    case 3:
        _enemySoldiers3.pushBack(u);
        break;
    case 4:
        _enemySoldiers4.pushBack(u);
        break;
    }
}

void GameScene::pushEnemyBuildingByID(Building* b, int id)
{
    switch (id)
    {
    case 1:
        _enemyBuildings1.pushBack(b);
        break;
    case 2:
        _enemyBuildings2.pushBack(b);
        break;
    case 3:
        _enemyBuildings3.pushBack(b);
        break;
    case 4:
        _enemyBuildings4.pushBack(b);
        break;
    }
}

Vector<Unit*>* GameScene::getSelectedSoldiers()
{
	return &_selectedSoldiers;
}

Vector<Unit*>* GameScene::getSoldiers()
{
	return &_soldiers;
}

Unit* GameScene::getSoldierByIndex(int index)
{
    for (auto& soldier : _soldiers)
    {
        if (soldier->getIndex() == index)
        {
            return soldier;
        }
    }
    return nullptr;
}

Vector<Building*>* GameScene::getBuildings()
{
	return &_buildings;
}

Vector<Unit*>* GameScene::getEnemySoldiersByID(int id)
{
    switch (id)
    {
    case 1:
        return &_enemySoldiers1;
        break;
    case 2:
        return &_enemySoldiers2;
        break;
    case 3:
        return &_enemySoldiers3;
        break;
    case 4:
        return &_enemySoldiers4;
        break;
    default:
        return nullptr;
        break;
    }
    return nullptr;
}

Unit* GameScene::getEnemySoldierByIdIndex(int id, int index)
{
    switch (id)
    {
    case 1:
        for (auto& soldier : _enemySoldiers1)
        {
            if (soldier->getIndex() == index)
            {
                return soldier;
            }
        }
        break;
    case 2:
        for (auto& soldier : _enemySoldiers2)
        {
            if (soldier->getIndex() == index)
            {
                return soldier;
            }
        }
        break;
    case 3:
        for (auto& soldier : _enemySoldiers3)
        {
            if (soldier->getIndex() == index)
            {
                return soldier;
            }
        }
        break;
    case 4:
        for (auto& soldier : _enemySoldiers4)
        {
            if (soldier->getIndex() == index)
            {
                return soldier;
            }
        }
        break;
    default:
        return nullptr;
    }
    return nullptr;
}

Vector<Building*>* GameScene::getEnemyBuildingsByID(int id)
{
    switch (id)
    {
    case 1:
        return &_enemyBuildings1;
        break;
    case 2:
        return &_enemyBuildings2;
        break;
    case 3:
        return &_enemyBuildings3;
        break;
    case 4:
        return &_enemyBuildings4;
        break;
    default:
        return nullptr;
    }
    return nullptr;
}

void GameScene::addMoney(int money)
{
	_money += money;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    // 显示金钱
    removeChild(_moneyCount);
    sprintf(_moneyStr, "%d", _money);
    _moneyCount = Label::createWithTTF(_moneyStr, "fonts/Marker Felt.ttf", 36);
    _moneyCount->setPosition(120, 50);
    _moneyCount->setColor(Color3B(255, 185, 15)); //颜色
    addChild(_moneyCount, 3);
}

void GameScene::decreaseMoney(int money)
{
	_money -= money;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    // 显示金钱
    removeChild(_moneyCount);
    sprintf(_moneyStr, "%d", _money);
    _moneyCount = Label::createWithTTF(_moneyStr, "fonts/Marker Felt.ttf", 36);
    _moneyCount->setPosition(120, 50);
    _moneyCount->setColor(Color3B(255, 185, 15)); //颜色
    addChild(_moneyCount, 3);
}

void GameScene::addPower(int power)
{
	_power += power;
	if (_power >= 0)
	{
		_isPowerEnough = true;
	}
	else
	{
		_isPowerEnough = false;
	}
    // updata power bar
    if (_power <= 0 || _totalPower == 0)
    {
        auto progressTo = ProgressTo::create(0.5f, 0);
        _powerBar->runAction(progressTo);
    }
    else
    {
        auto progressTo = ProgressTo::create(0.5f, _power*100 / _totalPower);
        _powerBar->runAction(progressTo);
    }
}

void GameScene::decreasePower(int power)
{
	_power -= power;
	if (_power >= 0)
	{
		_isPowerEnough = true;
	}
	else
	{
		_isPowerEnough = false;
	}
    // updata power bar
    if (_power <= 0 || _totalPower == 0)
    {
        auto progressTo = ProgressTo::create(0.5f, 0);
        _powerBar->runAction(progressTo);
    }
    else
    {
        auto progressTo = ProgressTo::create(0.5f, _power * 100 / _totalPower);
        _powerBar->runAction(progressTo);
    }
}

void GameScene::addTotalPower(int power)
{
	_totalPower += power;
}

void GameScene::decreaseTotalPower(int power)
{
	_totalPower -= power;
}

void GameScene::update(float time)
{
    Layer::update(time);

	//如果己方单位以全部被消灭，则广播自己死亡的消息
	if (_soldiers.size() == 0 && _buildings.size() == 0)
	{

		_gameEventDispatcher->removeAllEventListeners();
		//释放定时器
		this->unscheduleUpdate();
		this->unscheduleAllSelectors();


		_client->sendMessage(DEAD_MESSAGE, std::to_string(_localPlayerID));
		Director::getInstance()->replaceScene(TransitionFade::create(1, EndingScene::createScene(false)));
		return;
	}

	_manager->addMoneyUpdate();

    _manager->waitCreateBuilding();
    _manager->waitCreateSoldier();
    _manager->waitCreateCar();

    _manager->attack();
	_manager->getMoveController()->moveSoldiers();

	_manager->doCommands();

	scrollMap();
	showOnSmallMap();
	drawNode->clear();
	drawNode3->clear();

	if (_satelliteNum && _isPowerEnough)    //如果拥有卫星建筑且电量充足，则消除迷雾
	{
		hasFog = false;
	}
	else
	{
		hasFog = true;
	}
	if (hasFog) {
		makeFog();
	}
}

/*update by czd */
void GameScene::scrollMap() 
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto X = _cursorPosition.x;
	auto Y = _cursorPosition.y;
    Point mapPosition = _tileMap->getPosition();
	if (X < MINLENTH || _keyLeft) 
    {
		if (_tileMap->getPositionX() + SPEED < 0) 
        {
			_tileMap->setPosition(Vec2(SPEED, 0) + mapPosition);
            moveSpritesWithMap(Vec2(SPEED, 0));
		}
		else 
        {
			_tileMap->setPositionX(0);
            moveSpritesWithMap(Vec2(-mapPosition.x, 0));
		}
	}
    else if (X > visibleSize.width - MINLENTH || _keyRight)
    {
        if (_tileMap->getPositionX() - SPEED > -MAPX + visibleSize.width)
        {
            _tileMap->setPosition(mapPosition + Vec2(-SPEED, 0));
            moveSpritesWithMap(Vec2(-SPEED, 0));
        }
        else 
        {
            _tileMap->setPositionX(-MAPX + visibleSize.width);
            moveSpritesWithMap(Vec2(-MAPX + visibleSize.width - mapPosition.x, 0));
        }
    }
    // re get the map position
    mapPosition = _tileMap->getPosition();
	if (Y < MINLENTH || _keyDown) 
    {
		if (_tileMap->getPositionY() + SPEED < 0) 
        {
			_tileMap->setPosition(mapPosition + Point(0, SPEED));
            moveSpritesWithMap(Vec2(0, SPEED));
		}
		else 
        {
			_tileMap->setPositionY(0);
            moveSpritesWithMap(Vec2(0, -mapPosition.y));
		}
	}
	else if (Y > visibleSize.height - MINLENTH || _keyUp) 
    {
		if (_tileMap->getPositionY() - SPEED >  -MAPY + visibleSize.height)
        {
			_tileMap->setPosition(Vec2(0, -SPEED) + mapPosition);
            moveSpritesWithMap(Vec2(0, -SPEED));
		}
		else 
        {
			_tileMap->setPositionY(-MAPY + visibleSize.height);
            moveSpritesWithMap(Vec2(0, -MAPY + visibleSize.height - mapPosition.y));
		}
	}
}
//==========================战争迷雾======================================
void GameScene::makeFog() {
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();
	int temp = 10;
	
		for (int i = 0; i < temp; i++) {

			for (int j = 0; j < temp; j++) {
				if (fog[i][j] == 0) {
					Vec2 black = Point(i * 384, j * 384);
					Vec2 screenBlack = this->_tileMap->convertToWorldSpace(black);
					Vec2 point1[4];
					point1[0] = Vec2(screenBlack.x, screenBlack.y);
					point1[1] = Vec2(screenBlack.x, screenBlack.y + 384);
					point1[2] = Vec2(screenBlack.x + 384, screenBlack.y + 384);
					point1[3] = Vec2(screenBlack.x + 384, screenBlack.y);
					drawNode->drawPolygon(point1, 4, Color4F(0, 0, 0, 1), 1, Color4F(0, 0, 0, 1));

					//小地图上的黑块
					Vec2 black2 = Point(i * 30, j * 30);
					Vec2 point2[4];
					point2[0] = Vec2(black2.x, black2.y) + Vec2(visibleSize.x - small_mapX, visibleSize.y - small_mapY);
					point2[1] = Vec2(black2.x, black2.y + 30) + Vec2(visibleSize.x - small_mapX, visibleSize.y - small_mapY);
					point2[2] = Vec2(black2.x + 30, black2.y + 30) + Vec2(visibleSize.x - small_mapX, visibleSize.y - small_mapY);
					point2[3] = Vec2(black2.x + 30, black2.y) + Vec2(visibleSize.x - small_mapX, visibleSize.y - small_mapY);
					drawNode3->drawPolygon(point2, 4, Color4F(0, 0, 0, 1), 1, Color4F(0, 0, 0, 1));

				}
			}
		}

		for (auto& soldier : _soldiers) {
			Vec2 pos = soldier->getPosition();
			Vec2 truePos = this->_tileMap->convertToNodeSpace(pos);
			int x = truePos.x / 384;
			int y = truePos.y / 384;
			if (fog[x][y] == 0) {
				
				 fog[x - 1][y - 1] = 1; fog[x - 1][y] = 1; fog[x - 1][y + 1] = 1;
				 fog[x - 0][y - 1] = 1; fog[x - 0][y] = 1; fog[x - 0][y + 1] = 1;
				fog[x + 1][y - 1] = 1; fog[x + 1][y] = 1; fog[x + 1][y + 1] = 1; 
				
			}


		}
		for (auto& soldier : _buildings) {
			Vec2 pos = soldier->getPosition();
			Vec2 truePos = this->_tileMap->convertToNodeSpace(pos);
			int x = truePos.x / 384;
			int y = truePos.y / 384;
			if (fog[x][y] == 0) {
				fog[x - 1][y - 1] = 1; fog[x - 1][y] = 1; fog[x - 1][y + 1] = 1;
				fog[x - 0][y - 1] = 1; fog[x - 0][y] = 1; fog[x - 0][y + 1] = 1;
				fog[x + 1][y - 1] = 1; fog[x + 1][y] = 1; fog[x + 1][y + 1] = 1;
			}

		}

		
	
}






//========================显示小点点在小地图上==========================
void GameScene::showOnSmallMap() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	// my soldiers	
	drawNode2->clear();
	for (auto& soldier : _soldiers) {
		Vec2 pos = soldier->getPosition();
		Vec2 truePos = this->_tileMap->convertToNodeSpace(pos);
		float X = truePos.x / 3840 * small_mapX;
		float Y = truePos.y / 3840 * small_mapY;
		X = visibleSize.width - small_mapX + X;
		Y = visibleSize.height - small_mapY + Y;

		Vec2 point1[4];
		point1[0] = Vec2(X-1, Y-1);
		point1[1] = Vec2(X+1 , Y - 1);
		point1[2] = Vec2(X+1, Y + 1);
		point1[3] = Vec2(X - 1, Y + 1);
		drawNode2->drawPolygon(point1, 4, Color4F(0, 1, 0, 1), 1, Color4F(0, 1, 0, 1));
	}
	// my buildings
	for (auto& building : _buildings) {
		Vec2 pos = building->getPosition();
		Vec2 truePos = this->_tileMap->convertToNodeSpace(pos);
		float X = truePos.x / 3840 * small_mapX;
		float Y = truePos.y / 3840 * small_mapY;
		X = visibleSize.width - small_mapX + X;
		Y = visibleSize.height - small_mapY + Y;
		//drawNode2->clear();
		Vec2 point1[4];
		point1[0] = Vec2(X - 2, Y - 2);
		point1[1] = Vec2(X + 2, Y - 2);
		point1[2] = Vec2(X + 2, Y + 2);
		point1[3] = Vec2(X - 2, Y + 2);
		drawNode2->drawPolygon(point1, 4, Color4F(0, 1, 0, 1), 1, Color4F(0, 1, 0, 1));
	}

    for (int i = 1; i <= 4 ; ++i)
    {
        for (auto& soldier: *(getEnemySoldiersByID(i))) 
        {
            Vec2 pos = soldier->getPosition();
            Vec2 truePos = this->_tileMap->convertToNodeSpace(pos);

            /*if (fog[i][j] == 1) {*/
            float X = truePos.x / 3840 * small_mapX;
            float Y = truePos.y / 3840 * small_mapY;
            X = visibleSize.width - small_mapX + X;
            Y = visibleSize.height - small_mapY + Y;
            //drawNode2->clear();
            Vec2 point1[4];
            point1[0] = Vec2(X - 1, Y - 1);
            point1[1] = Vec2(X + 1, Y - 1);
            point1[2] = Vec2(X + 1, Y + 1);
            point1[3] = Vec2(X - 1, Y + 1);
            drawNode2->drawPolygon(point1, 4, Color4F(1, 0, 0, 1), 1, Color4F(1, 0, 0, 1));
            /*}*/

        }

        for (auto& building : *(getEnemyBuildingsByID(i))) 
        {
            Vec2 pos = building->getPosition();
            Vec2 truePos = this->_tileMap->convertToNodeSpace(pos);
            /*if (fog[i][j] == 1) {*/
            float X = truePos.x / 3840 * small_mapX;
            float Y = truePos.y / 3840 * small_mapY;
            X = visibleSize.width - small_mapX + X;
            Y = visibleSize.height - small_mapY + Y;
            //drawNode2->clear();
            Vec2 point1[4];
            point1[0] = Vec2(X - 2, Y - 2);
            point1[1] = Vec2(X + 2, Y - 2);
            point1[2] = Vec2(X + 2, Y + 2);
            point1[3] = Vec2(X - 2, Y + 2);
            drawNode2->drawPolygon(point1, 4, Color4F(1, 0, 0, 1), 1, Color4F(1, 0, 0, 1));
            /*}*/
        }
    }
	
}

void GameScene::moveSpritesWithMap(cocos2d::Vec2 direction)
{
    //============== my soldiers==============================
    for (auto& soldier : _soldiers)
    {
        soldier->setPosition(soldier->getPosition() + direction);
        if (!soldier->getGetDestination())
        {
            soldier->setDestination(soldier->getDestination() + direction);
        }
        std::vector<Point>::iterator iter2;
        for (iter2 = soldier->_route.begin(); iter2 != soldier->_route.end(); iter2++)
        {
            *iter2 += direction;
        }
    }
    //====================== my buildings==================================
    for (auto& building : _buildings)
    {
        building->setPosition(building->getPosition() + direction);
    }
    //========================= enemy soldiers==============================
    for (int i = 1; i <= 4; ++i)
    {
        for (auto& soldier : *(getEnemySoldiersByID(i)))
        {
            //log("move with map %f %f", soldier->getPosition().x, soldier->getPosition().y);
            //log("id %d", i);
            //log("direciton %f %f",direction.x, direction.y);
            soldier->setPosition(soldier->getPosition() + direction);
            if (!soldier->getGetDestination())
            {
                soldier->setDestination(soldier->getDestination() + direction);
            }
            std::vector<Point>::iterator iter2;
            for (iter2 = soldier->_route.begin(); iter2 != soldier->_route.end(); iter2++)
            {
                *iter2 += direction;
            }
        }
    }
    
    // ======================enemy buildings========================================
    for (int i = 1; i <= 4; ++i)
    {
        for (auto& building : *(getEnemyBuildingsByID(i)))
        {
            building->setPosition(building->getPosition() + direction);
        }
    }

    // barracks position
    if (_barracksNum)
    {
        _barracksPosition += direction;
    }
    // car factory position
    if (_carFactoryNum)
    {
        _carFactoryPosition += direction;
    }
    // sell building menu
    if (_isSellMenuExit)
    {
        _sellBuildingMenu->setPosition(_sellBuildingMenu->getPosition() + direction);
    }
}

bool GameScene::isCollision(cocos2d::Vec2 position)
{
    // turn PixelPosition to TileCoord
    Size mapSize = _tileMap->getMapSize();
    Size tileSize = _tileMap->getTileSize();
    auto mapPosition = _tileMap->convertToNodeSpace(position);
    if (mapPosition.x < 30 || mapPosition.y < 30 || mapPosition.x > MAPX - 30 
        || mapPosition.y > MAPY - 30) 
    {
        return true;
    }
    for (auto &building : *(this->getBuildings())) {
        auto X = position.x - building->getPositionX() + 100;
        auto Y = position.y - building->getPositionY() + 30;
        if (Y > -0.5*X && Y < 0.5*X && 0.5*X - 100 < Y && Y < 100 - 0.5*X)
        {
            return true;
        }
    }
    for (int i = 1; i <= 4; ++i)
    {
        for (auto &building : *(getEnemyBuildingsByID(i)))
        {
            auto X = position.x - building->getPositionX() + 100;
            auto Y = position.y - building->getPositionY() + 30;
            if (Y > -0.5*X && Y < 0.5*X && 0.5*X - 100 < Y && Y < 100 - 0.5*X)
            {
                return true;
            }
        }
    }
    position.x = static_cast<int>(mapPosition.x / tileSize.width);
    position.y = mapSize.height - static_cast<int>(mapPosition.y / tileSize.width) - 1;
    // get the GID of tile
    int tileGID = _barrier->getTileGIDAt(position);
    if (!tileGID) 
    {
        return false;
    }
    return true;
}

float GameScene::getTileSize()
{
	return _tileMap->getTileSize().width;
}

void GameScene::printTime(float dt)
{
    ++_time;
    removeChild(_timeCount);
    sprintf(_timeStr, "%d:%d:%d", _time / 3600, _time / 60, _time % 60);
    _timeCount = Label::createWithTTF(_timeStr, "fonts/Marker Felt.ttf", 28);
    _timeCount->setPosition(120, 100);
    _timeCount->setColor(Color3B(0, 0, 0)); //颜色
    addChild(_timeCount, 3);
}

void GameScene::sellBuildingCallBack()
{
    if (!_sellBuilding)
    {
        return;
    }

	_client->sendMessage(REMOVE_BUILDING, _manager->getRemoveBuildingMessage(_sellBuilding));

    // if is the base
    if (_sellBuilding->getBuildingTag() == BASE_TAG)
    {
        //turn base to base car
        //remove base
        _buildings.eraseObject(_sellBuilding, false);
        Vec2 position = _sellBuilding->getPosition();
        _buildingIndexDied[_sellBuilding->getIndex()] = 1;
        this->removeChild(_sellBuilding);
        //create base car
        Unit* baseCar = Unit::create(BASE_CAR_TAG, _localPlayerID, _unitIndex);
        ++_unitIndex;
        _gameEventDispatcher->addEventListenerWithSceneGraphPriority
            (_gameListener->clone(), baseCar);
        baseCar->setPosition(position);
        baseCar->setDestination(position);
        baseCar->setGetDestination(true);
        this->addChild(baseCar, 1);
        _isBaseExist = false;
        _soldiers.pushBack(baseCar);
		_client->sendMessage(CREATE_UNIT, _manager->getCreateUnitMessage(BASE_CAR_TAG, position));

        removeChild(_sellBuildingMenu);
        _sellBuilding = nullptr;
        _isSellMenuExit = false;

		//刷新Panel
		ptr_panel->setCurButton(ptr_panel->getCurCategoryTag());

        return;
    }
        
    Tag sellBuildingTag = _sellBuilding->getBuildingTag();
    _buildings.eraseObject(_sellBuilding);
    _buildingIndexDied[_sellBuilding->getIndex()] = 1;
    removeChild(_sellBuilding);
    _sellBuilding = nullptr;
    switch (sellBuildingTag)
    {
    case POWER_PLANT_TAG:
        decreasePowerPlant();            // 电厂数量减一
        _manager->resetPower();                          // 重置电量
        addMoney(buildingData::powerPlantCostMoney);
        break;

    case MINE_TAG:
        decreaseMine();                  // 矿场数量-1
        addPower(buildingData::mineCostPower);
        addMoney(buildingData::mineCostMoney);
        break;

    case BARRACKS_TAG:
        decreaseBarracks();
        if (getBarracksNum())
        {
            for (auto& building : _buildings)
            {
                if (building->getBuildingTag() == BARRACKS_TAG)
                {
                    setBarracksPosition(building->getPosition());
                }
            }
        }
        addPower(buildingData::barracksCostPower);
        addMoney(buildingData::barracksCostMoney);
        break;

    case CAR_FACTORY_TAG:
        decreaseCarFactory();
        if (getCarFactoryNum())
        {
            for (auto& building : _buildings)
            {
                if (building->getBuildingTag() == CAR_FACTORY_TAG)
                {
                    setCarFactoryPosition(building->getPosition());
                }
            }
        }
        addPower(buildingData::carFactoryCostPower);
        addMoney(buildingData::carFactoryCostMoney);
        break;

	case SATELLITE_TAG:
		decreaseSatellite();
		addPower(buildingData::satelliteCostPower);
		addMoney(buildingData::satelliteCostMoney);
		break;

    }

    removeChild(_sellBuildingMenu);
    _sellBuilding = nullptr;
    _isSellMenuExit = false;

	//刷新Panel
	ptr_panel->setCurButton(ptr_panel->getCurCategoryTag());

}

bool GameScene::inDiamond(cocos2d::Point center, float width,
    float height, cocos2d::Point position)
{
    Vec2 direction = position - center;
    //得到相对于中心点的坐标并转到第一象限.
    float x = fabs(direction.x);
    float y = fabs(direction.y);

    //根据x算出线上的y.
    float Y = (height * width - x * height) / width;

    log("Y: %f, y: %f", Y, y);

    if (Y > y)
    {
        return true;
    }
    else
    {
        return false;
    }
}
