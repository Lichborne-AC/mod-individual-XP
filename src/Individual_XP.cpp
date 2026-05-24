#include "ScriptMgr.h"
#include "Configuration/Config.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Player.h"
#include "Object.h"
#include "DataMap.h"

using namespace Acore::ChatCommands;

/*
Coded by Talamortis - For Azerothcore
Thanks to Rochet for the assistance
*/

uint32 MaxRate;
uint32 DefaultRate;

class PlayerXpRate : public DataMap::Base
{
public:
    PlayerXpRate() {}
    PlayerXpRate(uint32 XPRate) : XPRate(XPRate) {}
    uint32 XPRate = 1;
};

class Individual_XP : public PlayerScript
{
public:
    Individual_XP() : PlayerScript("Individual_XP") { }

    void OnPlayerLogin(Player* p) override
    {
        QueryResult result = CharacterDatabase.Query("SELECT `XPRate` FROM `individualxp` WHERE `CharacterGUID` = {}", p->GetGUID().GetCounter());
        if (!result)
        {
            p->CustomData.GetDefault<PlayerXpRate>("Individual_XP")->XPRate = DefaultRate;
        }
        else
        {
            Field* fields = result->Fetch();
            p->CustomData.Set("Individual_XP", new PlayerXpRate(fields[0].Get<uint32>()));
        }
            ChatHandler(p->GetSession()).SendSysMessage("This server is running the |cff4CFF00Individual XP |rmodule. Use .XP to see all the commands.");
    }

    void OnPlayerLogout(Player* p) override
    {
        if (PlayerXpRate* data = p->CustomData.Get<PlayerXpRate>("Individual_XP"))
        {
            uint32 rate = data->XPRate;
            CharacterDatabase.DirectExecute("REPLACE INTO `individualxp` (`CharacterGUID`, `XPRate`) VALUES ({}, {});", p->GetGUID().GetCounter(), rate);
        }
    }

    void OnPlayerGiveXP(Player* p, uint32& amount, Unit* /*victim*/, uint8 /*xpSource*/) override
    {
        if (PlayerXpRate* data = p->CustomData.Get<PlayerXpRate>("Individual_XP"))
            amount *= data->XPRate;
    }
};

class Individual_XP_command : public CommandScript
{
public:
    Individual_XP_command() : CommandScript("Individual_XP_command") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable IndividualXPCommandTable =
        {
            { "view",    HandleViewCommand,    SEC_PLAYER, Console::No },
            { "set",     HandleSetCommand,     SEC_PLAYER, Console::No },
            { "default", HandleDefaultCommand, SEC_PLAYER, Console::No },
            { "disable", HandleDisableCommand, SEC_PLAYER, Console::No },
            { "enable",  HandleEnableCommand,  SEC_PLAYER, Console::No },
        };

        static ChatCommandTable IndividualXPBaseTable =
        {
            { "xp", IndividualXPCommandTable },
        };

        return IndividualXPBaseTable;
    }
    
    // View Command
    static bool HandleViewCommand(ChatHandler* handler, char const* args)
    {
        if (*args)
            return false;
          
        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;
        
        if (me->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN))
        {
            me->GetSession()->SendAreaTriggerMessage("Your XP is currently disabled. Do .Xp Enable to re-enable it.");
        }
        else
        {
            me->GetSession()->SendAreaTriggerMessage("Your current XP rate is %u", me->CustomData.GetDefault<PlayerXpRate>("Individual_XP")->XPRate);
        }
        return true;
    }
    
    // Set Command
    static bool HandleSetCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        uint32 rate = (uint32)atol(args);
        if (rate > MaxRate)
            return false;

        me->CustomData.GetDefault<PlayerXpRate>("Individual_XP")->XPRate = rate;
        me->GetSession()->SendAreaTriggerMessage("You have updated your XP rate to %u", rate);
        return true;
    }
    
    // Disable Command
    static bool HandleDisableCommand(ChatHandler* handler, char const* args)
    {
        if (*args)
            return false;
          
        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;
        
        // Turn Disabled On But Don't Change Value...
        me->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        me->GetSession()->SendAreaTriggerMessage("You have Disabled your XP gain.");
        return true;
    }
    
    // Enable Command
    static bool HandleEnableCommand(ChatHandler* handler, char const* args)
    {
        if (*args)
            return false;
        
        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;
          
        me->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        me->GetSession()->SendAreaTriggerMessage("You have enabled your XP gain.");
        return true;
    }
    
    // Default Command
    static bool HandleDefaultCommand(ChatHandler* handler, char const* args)
    {
        if (*args)
            return false;
          
        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;
        
        me->CustomData.GetDefault<PlayerXpRate>("Individual_XP")->XPRate = DefaultRate;
        me->GetSession()->SendAreaTriggerMessage("You have restored your XP rate to the default value of %u", DefaultRate);
        return true;
    }
};

class Individual_XP_conf : public WorldScript
{
public:
    Individual_XP_conf() : WorldScript("Individual_XP_conf_conf") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        MaxRate = sConfigMgr->GetOption<uint32>("MaxXPRate", 10);
        DefaultRate = sConfigMgr->GetOption<uint32>("DefaultXPRate", 1);
    }
};

void AddIndividual_XPScripts()
{
    new Individual_XP();
    new Individual_XP_conf();
    new Individual_XP_command();
}
