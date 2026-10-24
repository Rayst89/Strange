#include "Valve.hpp"
#include "Source.hpp"
#include "Config.hpp"
#include "Entity.hpp"
#include "Player.hpp"

class cGameEventManager : public IGameEventListener2
{
public:
	void FireGameEvent( IGameEvent *event );
	void RegisterSelf();
	void RemoveListeners();
};
