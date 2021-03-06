#pragma once
#include "th_player.h"
#include "strategy/th_laser_expander.h"


class th15_player : public th_player
{
	th_laser_expander tle;
public:
	th15_player() : th_player(gs_addr{ (uint8_t*)0x004E9BB8,(uint8_t*)0x4E6F28 }), tle(10, 30, 6) {}
	~th15_player() = default;

	void onInit() override;
	void onTick() override;
	void onBeginTick() override;
	void onAfterTick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handleInput(const BYTE diKeys[256], const BYTE press[256]) override;
	void onEnableChanged(bool enable) override;

private:
	/**
	 * Note: the size for players in th15 is radius, not diameter.
	 */
	entity getPlayerEntity() override;
};
