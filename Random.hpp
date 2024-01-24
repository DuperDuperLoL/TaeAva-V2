#pragma once
class Random
{
private:
    ConfigLoader* m_configLoader;
    XDisplay* m_display;
    Level* m_level;
    LocalPlayer* m_localPlayer;
    std::vector<Player*>* m_players;
public:
    Random(ConfigLoader* configLoader,
          XDisplay* display,
          Level* level,
          LocalPlayer* localPlayer,
          std::vector<Player*>* players
          )
    {
        m_configLoader = configLoader;
        m_display = display;
        m_level = level;
        m_localPlayer = localPlayer;
        m_players = players;
    }
    //==========---------[SUPERGLIDE]-------------==================
    void superGlide(){
        while (m_display->keyDown(XK_W))
        {
            static float startjumpTime = 0;
            static bool startSg = false;
            static float traversalProgressTmp = 0.0;
 
            float worldtime = mem::Read<float>(m_localPlayer->base + OFFSET_TIME_BASE); // Current time
            float traversalStartTime = mem::Read<float>(m_localPlayer->base + OFFSET_TRAVERSAL_START_TIME); // Time to start wall climbing
            float traversalProgress = mem::Read<float>(m_localPlayer->base + OFFSET_TRAVERSAL_PROGRESS); // Wall climbing, if > 0.87 it is almost over.
            auto HangOnWall = -(traversalStartTime - worldtime);
 
            if (HangOnWall > 0.1 && HangOnWall < 0.12)
            {
                mem::Write<int>(OFF_REGION + OFF_IN_JUMP + 0x8, 4);
            }
            if (traversalProgress > 0.87f && !startSg && HangOnWall > 0.1f && HangOnWall < 1.5f)
            {
                //start SG
                startjumpTime = worldtime;
                startSg = true;
            }
            if (startSg)
            {
                //printf ("sg Press jump\n");
                mem::Write<int>(OFF_REGION + OFF_IN_JUMP + 0x8, 5);
                while (mem::Read<float>(m_localPlayer->base + OFFSET_TIME_BASE) - startjumpTime < 0.011);
                {
                    mem::Write<int>(OFF_REGION + OFF_IN_DUCK + 0x8, 6);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    mem::Write<int>(OFF_REGION + OFF_IN_JUMP + 0x8, 4);
                    std::this_thread::sleep_for(std::chrono::milliseconds(600));
                }
                startSg = false;
                break;
            }
        }
 
        // Automatic wall jump
        int wallJumpNow = 0;
 
        static float onWallTmp = 0;
        float onWall = mem::Read<float>(m_localPlayer->base + OFFSET_WALL_RUN_START_TIME);
        if (onWall > onWallTmp + 0.1) // 0.1
        {
            if (mem::Read<int>(OFF_REGION + OFFSET_IN_FORWARD) == 0)
            {
                wallJumpNow = 1;
                //printf("wall jump Press jump\n");
                mem::Write<int>(OFF_REGION + OFF_IN_JUMP + 0x8, 5);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                mem::Write<int>(OFF_REGION + OFF_IN_JUMP + 0x8, 4);
            }
        }
        onWallTmp = onWall;
 
        static float onEdgeTmp = 0;
        float onEdge = mem::Read<float>(m_localPlayer->base + OFFSET_TRAVERSAL_PROGRESS);
        if (onEdge > onEdgeTmp + 0.1) // 0.1
        {
            if (mem::Read<int>(OFF_REGION + OFFSET_IN_FORWARD) == 0)
            {
                wallJumpNow = 2;
                //printf("wall jump onEdge Press jump\n");
                mem::Write<int>(OFF_REGION + OFF_IN_JUMP + 0x8, 5);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                mem::Write<int>(OFF_REGION + OFF_IN_JUMP + 0x8, 4);
            }
        }
        onEdgeTmp = onEdge;    
    }
   
     //==================---------------[SPECTATORVIEW]-------------------==================
    void spectatorView()
    {
        if(!m_level->playable) return;
        int spectatorcount = 0;   
        std::vector<std::string> spectatorlist;
        if(m_configLoader->FEATURE_SPECTATOR_ON){
            for (int i = 0; i < m_players->size(); i++)
            { 
                Player *p = m_players->at(i);          
                float targetyaw = p->view_yaw; // get Yaw player
                float localyaw= m_localPlayer->local_yaw; // get Yaw LocalPlayer

                if (targetyaw == localyaw && p->currentHealth == 0){
                    spectatorcount++;
                    std::string namePlayer = p->getPlayerName();    
                    spectatorlist.push_back(namePlayer);
                }            
            }
            const auto spectatorlist_size = static_cast<int>(spectatorlist.size());
           
            if (spectatorcount > 0){
                {   
                    std::string spectatorsname = "";
                    for (int i = 0; i < spectatorlist_size; i++)   
                        //spectatorsname = spectatorsname  + spectatorlist.at(i).c_str() + "(" + p->GetPlayerLevel() + ")[" + p->getPlayerModelName() + "] - ";
                        spectatorsname = spectatorsname  + spectatorlist.at(i).c_str() + " - ";

                    std::string buffer;
                    buffer = util::getTime();
                    std::cout << "\r" << "["
                        << buffer << "] \033[31;1;4m"
                        << spectatorcount << " SPECTATOR(S)\033[0m: " 
                        << spectatorsname << "                                                    "
                        << std::flush;
                }
            }              
        }      
    }   
};
