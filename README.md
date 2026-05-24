# mod-indivisual-XP

#This Module is for Azerothcore -

# How to install

1) clone this module into the modules directory of the main source
2) apply the provided sql in the character database
3) rerun cmake
4) compile.

# Commands

All commands are available to every player (`SEC_PLAYER`). The base command is `.xp` (case-insensitive). Bots route the same commands through their handler too.

| Command | Description |
|---------|-------------|
| `.xp view` | Show your current XP rate. If XP gain is disabled, displays a reminder that you can re-enable it with `.xp enable`. |
| `.xp set <rate>` | Set your personal XP rate. Refused if `<rate>` exceeds `MaxXPRate` from the config. |
| `.xp default` | Reset your XP rate to the server-configured default (`DefaultXPRate`). |
| `.xp disable` | Disable XP gain entirely (sets `PLAYER_FLAGS_NO_XP_GAIN`). XP rate is preserved while disabled. |
| `.xp enable` | Re-enable XP gain after `.xp disable`. |

Each player's rate is persisted in the `individualxp` table on character logout and loaded on login. A login message reminds the player that the module is active.

# Configuration

Edit `Individual-XP.conf` in your `etc/modules/` directory (copy from the provided `.dist` file).

| Option | Default | Description |
|--------|---------|-------------|
| `MaxXPRate` | `10` | Highest rate a player may pass to `.xp set`. |
| `DefaultXPRate` | `1` | Rate assigned to a new character on first login, and the rate `.xp default` restores. |

# Show your a appreciation:

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SBJFTAJKUNEXC)
