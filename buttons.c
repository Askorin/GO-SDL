#include "headers/buttons.h"

toggle_button_group_t init_toggle_btn_grp(int len, toggle_button_t** toggle_button_ptrs, int default_val)
{
    toggle_button_group_t group = {
        .len = len,
        .toggle_button_ptrs = toggle_button_ptrs,
        .val = default_val
    };

    return group;
}

bool press_toggle_btn(toggle_button_group_t* btn_grp, int idx)
{
    bool success = false;
    int btn_len = btn_grp->len;
    if (idx < btn_len) {
        /* Conseguimos el botón a presionar en el grupo */
        toggle_button_t* btn_to_press = btn_grp->toggle_button_ptrs[idx];
        /* Lo seteamos a true en toggle */
        btn_to_press->toggle = true;
        /* Ahora el valor del grupo es el valor del botón presionado */
        btn_grp->val = btn_to_press->val;
        
        /* Desactivamos el resto de los botones */
        for (int i = 0; i < btn_len; ++i) {
            if (i == idx) continue;
            toggle_button_t* btn_to_unpress = btn_grp->toggle_button_ptrs[i];
            btn_to_unpress->toggle = false;
        }
        success = true;
    }
    return success;
}


