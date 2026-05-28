# Stateful Mod

Reserve state in `mod.json`:

```json
{
  "id": "my_stateful_mod",
  "name": "My Stateful Mod",
  "version": "1.0.0",
  "requiresSdk": ">=1",
  "stateVersion": 1,
  "stateBytes": 12
}
```

Access it from C:

```c
#include "mod_sdk.h"

struct MyState
{
    u16 counter;
};

void MyState_Tick(void)
{
    struct MyState *state = ModState_GetBlock("my_stateful_mod", sizeof(*state), 1);

    if (state == NULL)
        return;
    if (ModState_NeedsMigration("my_stateful_mod"))
        ModState_MarkMigrated("my_stateful_mod");

    state->counter++;
    ModState_BumpRevision();
}
```
