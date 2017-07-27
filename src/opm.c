#define OPM_PHASE 

const int opm_height=30, opm_half_width=5;

void opm_start_level(int p)
{
    players[p].vz = -10;
    players[p].vx = 3;
}

static inline int opm_run_charge(int p, float dt)
{
    if (GAMEPAD_PRESSED(p, Y))
    {
        if (players[p].run_charge < 256)
            players[p].run_charge += 0.5 + 0.5*dt;
        else
            players[p].run_charge = 256;
        return 1;
    }
    else
    {
        players[p].run_charge -= 0.5 * players[p].run_charge * (0.5*dt + 0.5);
        return 0;
    }
}
    
static const uint8_t opm_next_frames[ANIM_COUNT] = {
    [ANIM_IDLE_0]=ANIM_IDLE_1,
    [ANIM_IDLE_1]=ANIM_IDLE_0,
    [ANIM_GUARD_R]=ANIM_IDLE_0,
    [ANIM_GUARD_L]=ANIM_IDLE_1,
    [ANIM_WALK_R_0]=ANIM_WALK_R_1,
    [ANIM_WALK_R_1]=ANIM_WALK_L_0,
    [ANIM_WALK_L_0]=ANIM_WALK_L_1,
    [ANIM_WALK_L_1]=ANIM_WALK_R_0,
    [ANIM_RUN_R_0]=ANIM_WALK_R_1,
    [ANIM_RUN_R_1]=ANIM_WALK_L_0,
    [ANIM_RUN_L_0]=ANIM_WALK_L_1,
    [ANIM_RUN_L_1]=ANIM_WALK_R_0,
    [ANIM_CROUCH_R]=ANIM_IDLE_0,
    [ANIM_CROUCH_L]=ANIM_IDLE_1,
    [ANIM_FLY_0]=ANIM_FLY_1,
    [ANIM_FLY_1]=ANIM_FLY_0,
    [ANIM_PUNCH_R_0]=ANIM_PUNCH_R_1,
    [ANIM_PUNCH_R_1]=ANIM_WALK_R_0,
    [ANIM_PUNCH_L_0]=ANIM_PUNCH_L_1,
    [ANIM_PUNCH_L_1]=ANIM_WALK_L_0,
    [ANIM_KICK_R_0]=ANIM_KICK_R_1,
    [ANIM_KICK_R_1]=ANIM_IDLE_0,
    [ANIM_KICK_L_0]=ANIM_KICK_L_1,
    [ANIM_KICK_L_1]=ANIM_IDLE_1,
    [ANIM_GRAB]=ANIM_IDLE_1,
    [ANIM_GRAB_THROW]=ANIM_IDLE_1,
    [ANIM_GRAB_PUNCH]=ANIM_GRAB,
    [ANIM_GRAB_KICK]=ANIM_GRAB,
    [ANIM_USER_0]=ANIM_IDLE_1,
    [ANIM_USER_1]=ANIM_IDLE_0,
    [ANIM_HURT]=ANIM_IDLE_0,
    [ANIM_BAD_HURT]=ANIM_HURT,
};

static const float opm_frame_rates[ANIM_COUNT] = {
    [ANIM_IDLE_0]=0.125,
    [ANIM_IDLE_1]=0.125,
    [ANIM_GUARD_R]=0.5,
    [ANIM_GUARD_L]=0.5,
    [ANIM_WALK_R_0]=0.2,
    [ANIM_WALK_R_1]=0.2,
    [ANIM_WALK_L_0]=0.2,
    [ANIM_WALK_L_1]=0.2,
    [ANIM_RUN_R_0]=0.4,
    [ANIM_RUN_R_1]=0.4,
    [ANIM_RUN_L_0]=0.4,
    [ANIM_RUN_L_1]=0.4,
    [ANIM_CROUCH_R]=0.1,
    [ANIM_CROUCH_L]=0.1,
    [ANIM_FLY_0]=0.2,
    [ANIM_FLY_1]=0.2,
    [ANIM_PUNCH_R_0]=100.0,
    [ANIM_PUNCH_R_1]=0.1,
    [ANIM_PUNCH_L_0]=100.0,
    [ANIM_PUNCH_L_1]=0.1,
    [ANIM_KICK_R_0]=100.0,
    [ANIM_KICK_R_1]=100.0,
    [ANIM_KICK_L_0]=100.0,
    [ANIM_KICK_L_1]=100.0,
    [ANIM_GRAB]=10.0,
    [ANIM_GRAB_THROW]=10.0,
    [ANIM_GRAB_PUNCH]=10.0,
    [ANIM_GRAB_KICK]=10.0,
    [ANIM_USER_0]=0.1,
    [ANIM_USER_1]=0.1,
    [ANIM_HURT]=0.5,
    [ANIM_BAD_HURT]=0.5,
};

void opm_ground(int p, float dt)
{
    int from_frame = ANIM_FROM(p);
    int to_frame = ANIM_TO(p);
    int next_frame = opm_next_frames[to_frame];
    static float frame_rate = 0.25;
    if (to_frame/4 != ANIM_PUNCH_R_0/4 || to_frame % 2 == 0)
        frame_rate = opm_frame_rates[to_frame];
    //if (from_frame == ANIM_PUNCH_R_1 || from_frame == ANIM_PUNCH_L_1)
    //    message("to frame %d/from punching frame %d; at rate %f\n", to_frame, from_frame, frame_rate);

    int slow_down_time = opm_run_charge(p, dt);
    if (vga_frame % 64 == 0)
        message("run %f at vx=%f\n", players[p].run_charge, players[p].vx);
    if (GAMEPAD_PRESSED(p, L))
    {
        if (players[p].jump_charge == 0)
            animation_interrupt(p, ANIM_CROUCH_R);
        players[p].jump_charge += 1.0f/16.0f + 0.5*dt;
        if (players[p].jump_charge >= 256)
        {
            players[p].jump_charge = 256;
            goto jump_now;
        }
        next_frame = ANIM_CROUCH_R;
        frame_rate = opm_frame_rates[ANIM_CROUCH_R];
        // double ding to velocity
        players[p].vx -= 0.5 * players[p].vx * dt;
        players[p].vy -= 0.5 * players[p].vy * dt;
    }
    else if (players[p].jump_charge)
    {
        jump_now:
        message("jump %f\n", players[p].jump_charge);
        players[p].vx *= 2.0;
        players[p].vy *= 2.0;
        players[p].vz = -players[p].jump_charge;
        players[p].jump_charge = 0;
        animation_interrupt(p, ANIM_FLY_0);
        frame_rate = opm_frame_rates[ANIM_FLY_0];
        next_frame = ANIM_FLY_1;
        // the above line does nothing if we return...
        return;
    }
    
    players[p].vx -= 0.5 * players[p].vx * dt;
    players[p].vy -= 0.5 * players[p].vy * dt;
    
    if (GAMEPAD_PRESSED(p, B))
    {
        if (players[p].punch_charge == 0)
        {
            players[p].punch_charge += 0.5 + dt;
            animation_interrupt(p, ANIM_PUNCH_R_0 + 2 - ((from_frame%4)/2)*2);
            if (dt != 1)
            {
                if (from_frame/4 == ANIM_PUNCH_R_0/4)
                    message("was punching before\n");
            }
            return;
        }
        else
        {
            players[p].punch_charge += 0.5 + dt;
            if (players[p].punch_charge > 256)
            {
                players[p].punch_charge = 256;
                goto punch_now;
            }
            next_frame = to_frame;
        }
    }
    else if (players[p].punch_charge)
    {
        punch_now:
        next_frame = ((next_frame%4)/2)*2;
        animation_interrupt(p, ANIM_PUNCH_R_1 + next_frame);
        frame_rate = 8*players[p].punch_charge;
        players[p].punch_charge = 0;
        next_frame = ANIM_PUNCH_R_0 + next_frame;
    }
    else if (GAMEPAD_PRESSED(p, right))
    {
        players[p].vx += 1 + 0.05*players[p].run_charge;
        if (vga_frame % 64 == 0)
            message("vx is %f\n", players[p].vx);
        if (GAMEPAD_PRESSED(p, L))
        {
            if (from_frame != ANIM_CROUCH_L)
                next_frame = ANIM_CROUCH_L;
            if (fabs(players[p].vx) < 10.0)
                frame_rate *= fabs(players[p].vx)/2.0;
            else if (dt == 1.0)
                frame_rate *= 10.0/2.0;
        }
        else if (players[p].vx > 10.0)
        {
            if (dt == 1.0)
                next_frame = ANIM_RUN_R_0 + next_frame%4;
            else
                goto walk;
        }
        else
        {
            if (dt == 1.0)
                frame_rate *= players[p].vx/3.0;
            walk:
            next_frame = ANIM_WALK_R_0 + next_frame%4;
        }
        slow_down_time = 0;
    }
    else if (GAMEPAD_PRESSED(p, left))
    {
        players[p].vx -= 1 + 0.05*players[p].run_charge;
        slow_down_time = 0;
        if (GAMEPAD_PRESSED(p, L))
        {
            if (from_frame != ANIM_CROUCH_L)
                next_frame = ANIM_CROUCH_L;
            if (fabs(players[p].vx) < 10.0)
                frame_rate *= fabs(players[p].vx)/2.0;
            else if (dt == 1.0)
                frame_rate *= 10.0/2.0;
        }
        else
            next_frame = ANIM_WALK_R_0 + (next_frame+2)%4;
    }
    else if (!(gamepad_buttons[p] & (gamepad_up | gamepad_down)))
    {
        if (next_frame/4 == ANIM_WALK_R_0/4)
        {
            if (players[p].vx < 2.0)
                next_frame %= 2; 
        }
        else if (next_frame/4 == ANIM_RUN_R_0/4)
        {
            if (players[p].vx < 10.0)
                next_frame -= 4;
        }
        goto after_movement;
    }
    if (GAMEPAD_PRESSED(p, up))
    {
        players[p].vy -= 1 + 0.03*players[p].run_charge;
        slow_down_time = 0;
        if (GAMEPAD_PRESSED(p, L))
        {
            if (from_frame != ANIM_CROUCH_L)
                next_frame = ANIM_CROUCH_L;
            if (fabs(players[p].vy) < 10.0)
                frame_rate *= fabs(players[p].vy)/2.0;
            else if (dt == 1.0)
                frame_rate *= 10.0/2.0;
        }
        else if (!(gamepad_buttons[p] & (gamepad_left | gamepad_right)))
        {
            if (fabs(players[p].vx) > 10.0 && dt == 1.0)
                next_frame = ANIM_RUN_R_0 + next_frame%4;
            else
                next_frame = ANIM_WALK_R_0 + next_frame%4;
        }
    }
    else if (GAMEPAD_PRESSED(p, down))
    {
        players[p].vy += 1 + 0.03*players[p].run_charge;
        slow_down_time = 0;
        if (GAMEPAD_PRESSED(p, L))
        {
            if (from_frame != ANIM_CROUCH_L)
                next_frame = ANIM_CROUCH_L;
            if (fabs(players[p].vy) < 10.0)
                frame_rate *= fabs(players[p].vy)/2.0;
            else if (dt == 1.0)
                frame_rate *= 10.0/2.0;
        }
        else if (!(gamepad_buttons[p] & (gamepad_left | gamepad_right)))
        {
            if (fabs(players[p].vx) > 10.0 && dt == 1.0)
                next_frame = ANIM_RUN_R_0 + next_frame%4;
            else
                next_frame = ANIM_WALK_R_0 + next_frame%4;
        }
    }

    after_movement:
    if (slow_down_time)
        physics_slow_time(players[p].run_charge);

    // here we see part of saitama's super power:
    // regardless of bullet time he moves (relative to slower time) as if it was regular time.
    frame_rate /= dt;
    // the other part is invincibility.
    
    animation_tween(p, dt, frame_rate, next_frame);
}

void opm_air(int p, float dt)
{
    if (vga_frame % 32 == 0)
        message("fly %f at vx=%f\n", players[p].run_charge, players[p].vx);
    if (opm_run_charge(p, dt) && !GAMEPAD_PRESSED(p, dpad))
        physics_slow_time(players[p].run_charge);

    int to_frame = ANIM_TO(p);
    if (to_frame/2 == ANIM_FLY_0/2)
        animation_tween(p, dt, 0.1, ANIM_FLY_0 | (1-(to_frame&1)));
    else
        animation_tween(p, dt, 0.1, ANIM_FLY_0);
}

void opm_line(int p)
{
    if (vga_line + opm_height <= players[p].iy || vga_line > players[p].iy)
    {
        if (vga_line == 0 && players[p].iy < opm_height)
            message("out of line: %d %d\n", vga_line, players[p].iy);
        return;
    }
    uint16_t *dst = &draw_buffer[players[p].ix - opm_half_width];
    if (dst < draw_buffer)
        dst = draw_buffer;
    uint16_t *dst_max = &draw_buffer[players[p].ix + opm_half_width];
    if (dst_max >= draw_buffer + SCREEN_W)
        dst_max = draw_buffer + SCREEN_W;
    while (dst < dst_max)
        *dst++ = RGB(255, 255, 0);
}
