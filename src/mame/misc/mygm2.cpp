// license::BSD-3-Clause
// copyright-holders:flama12333
/*************************************************************************


**************************************************************************/

#include "emu.h"

#include "cpu/z80/z80.h"
#include "sound/tt5665.h"
#include "machine/nvram.h"
#include "speaker.h"
#include "screen.h"
#include "tilemap.h"
#include "emupal.h"



namespace {

class MYGM2_state : public driver_device
{
public:
	MYGM2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_adpcm(*this, "tt5665")

	{ }

	void MYGM2(machine_config &config);
    void init_dec();

protected:
	virtual void machine_start() override;
	required_device<tt5665_device> m_adpcm;

private:
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void io_map(address_map &map);
	void program_map(address_map &map);

};

uint32_t MYGM2_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return 0;
}


static INPUT_PORTS_START( guermagic2 )
	PORT_START("IN0")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_Q)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_W)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_E)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(1)  // Will cause error 33
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_T)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_U)
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_I)
	
	PORT_START("IN1")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_A)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_S)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_D)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_F)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_G)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_H)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_J)
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYPAD) PORT_CODE(KEYCODE_K)
	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_0_PAD)  
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_7_PAD)
	PORT_START("IN3")
    PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x01, "DSW1:1") // if set off will cause error 44. if set on, will cause error 55
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x02, "DSW1:2")
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "DSW1:3")
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "DSW1:4")
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x10, "DSW1:5")
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "DSW1:6")
	PORT_DIPUNKNOWN_DIPLOC( 0x40, 0x40, "DSW1:7")
	PORT_DIPUNKNOWN_DIPLOC( 0x80, 0x80, "DSW1:8")
	PORT_START("IN4")
    PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x01, "DSW2:1")
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x02, "DSW2:2")
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "DSW2:3")
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "DSW2:4")
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x10, "DSW2:5")
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "DSW2:6")
	PORT_DIPUNKNOWN_DIPLOC( 0x40, 0x40, "DSW2:7")
	PORT_DIPUNKNOWN_DIPLOC( 0x80, 0x80, "DSW2:8")

	
INPUT_PORTS_END


void MYGM2_state::program_map(address_map &map)
{
	map(0x0000, 0xe7ff).rom();
	map(0xe800, 0xefff).ram().share("unk1");
	map(0xf000, 0xf7ff).ram().share("unk2");
    map(0xf800, 0xffff).ram().share("nvram");


}

void MYGM2_state::io_map(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x00).nopw(); // ??? 
    map(0x01, 0x01).portr("IN0");
    map(0x02, 0x02).rw(m_adpcm,  FUNC(tt5665_device::read), FUNC(tt5665_device::write));
    map(0x03, 0x03).nopw(); // ??? 
    map(0x04, 0x04).nopw(); // ???
	map(0x10, 0x10).portr("IN1");
	map(0x11, 0x11).portr("IN2");
	map(0x12, 0x12).portr("IN3");
	map(0x13, 0x13).portr("IN4");



}

void MYGM2_state::init_dec()
{
    uint8_t *const rom = memregion("maincpu")->base();
    std::vector<uint8_t> buffer(0x10000);

    memcpy(&buffer[0], rom, 0x10000);

    for (int i = 0; i < 0x10000; i++)
        rom[i] = buffer[bitswap<24>(i, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 12, 13, 11, 10, 8, 9, 7, 6, 4, 5, 2, 3, 0, 1)];
} 

static GFXDECODE_START( gfx ) // Wrong.
GFXDECODE_ENTRY( "gfx", 0, gfx_16x16x8_raw, 0, 16 )
GFXDECODE_END


void MYGM2_state::machine_start()
{
}

void MYGM2_state::MYGM2(machine_config &config)
{

	/* basic machine hardware */
    z80_device &maincpu(Z80(config, "maincpu", XTAL(8'000'000)));
    maincpu.set_addrmap(AS_PROGRAM, &MYGM2_state::program_map);
	maincpu.set_addrmap(AS_IO, &MYGM2_state::io_map);
    maincpu.set_vblank_int("screen", FUNC(MYGM2_state::irq0_line_hold));
	
    maincpu.set_periodic_int(FUNC(MYGM2_state::nmi_line_pulse), attotime::from_hz(8 * 60)); // Guessed. TODO: find IRQ source
    NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
	TT5665(config, "tt5665", XTAL(20'000'000) / 12, tt5665_device::ss_state::SS_LOW, 0).add_route(1, "mono", 1.0);   // Pin, divisor not verified.
    
	PALETTE(config, "palette").set_entries(0x100); // wrong
    GFXDECODE(config, "gfxdecode", "palette", gfx);
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	//screen.set_size(64*5, 20*12);
	screen.set_size(320, 240);
	screen.set_visarea_full();
	screen.set_screen_update(FUNC(MYGM2_state::screen_update));
	screen.set_palette("palette");


}

ROM_START( guermagic2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tsk_mp.u2", 0x00000, 0x10000, CRC(5F1824E8) SHA1(603a0ddce6a1341542e82d59396cdfc5df7fbc49) )
	
	ROM_REGION( 0x400000, "gfx", 0 )
	ROM_LOAD( "tsk_pp.u10b", 0x00000, 0x400000, CRC(494C98A1) SHA1(b9f613d3aa1ec81a773e9fbe7f54b98f7991b140) )

	ROM_REGION( 0x400000, "tt5665", 0 )
	ROM_LOAD( "tsk_vp.us2", 0x0000, 0x400000, CRC(94C0D994) SHA1(843398697953a93b974f7311d4b968bc60229f7a) )

ROM_END

} // anonymous namespace


//    YEAR  NAME        PARENT   MACHINE   INPUT       STATE        INIT        ROT   COMPANY                        FULLNAME               FLAGS
GAME( 2013, guermagic2, 0,       MYGM2,    guermagic2, MYGM2_state, init_dec,   ROT0, "Ming-Yang Electronic / TSK", "GUERREROS MAGICOS II", MACHINE_NOT_WORKING )
