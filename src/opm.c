#define OPM_PHASE 

#define CAPE_LENGTH 15.0f

const int opm_height=30, opm_half_width=5;

void opm_start_level(int p)
{
    players[p].vz = -10;
    players[p].vx = 3;
    players[p].custom = 4;
    int k=32*p + 18;
    for (int dk=0; dk<2; ++dk)
    {
        quads[k+dk].x = players[p].x;
        quads[k+dk].y = players[p].y;
        quads[k+dk].z = players[p].z;
        quads[k+dk].vx = 0;
        quads[k+dk].vy = 0;
        quads[k+dk].vz = 0;
        quads[k+dk].color = RGB(255,255,255);
        quads[k+dk].edge_color = RGB(200,200,200);
        quads[k+dk].lifetime = 0;
    }
    draw_add_projectile(k, k+1);
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
    //int from_flipped = from_frame & ANIM_FACE_LEFT;
    from_frame &= ~ANIM_FACE_LEFT;
    int to_frame = ANIM_TO(p);
    int to_flipped = to_frame & ANIM_FACE_LEFT;
    to_frame &= ~ANIM_FACE_LEFT;
    int next_frame = opm_next_frames[to_frame];
    int next_flipped = to_flipped;

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
            animation_interrupt(p, ANIM_CROUCH_R | next_flipped);
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
        animation_interrupt(p, ANIM_FLY_0 | next_flipped);
        frame_rate = opm_frame_rates[ANIM_FLY_0];
        next_frame = ANIM_FLY_1;
        // the above line does nothing if we return...
        return;
    }
    
    players[p].vx -= 0.5 * players[p].vx * dt;
    players[p].vy -= 0.5 * players[p].vy * dt;
    
    if (GAMEPAD_PRESSED(p, B))
    {
        players[p].jump_charge = 0;
        if (players[p].punch_charge == 0)
        {
            if (dt != 1 && from_frame/4 == ANIM_PUNCH_R_0/4)
            {
                to_frame = ANIM_PUNCH_R_1 + 2 - ((from_frame%4)/2)*2;
                animation_interrupt(p, to_frame | next_flipped);
                animation_tween(p, dt, 5.0/dt, (to_frame & (~1)) | next_flipped);
                message("consecutive normal punches to frame %d at rate %f\n", to_frame, 5.0/dt);
                players[p].custom += 2;
                if (players[p].custom >= 16 - 1)
                    players[p].custom = 4;
                int k = 32*p + 16 + players[p].custom;
                int dy = 1 + 3*(rand()%6);
                int draw_indices[2] = {quads[k].draw_index, quads[k+1].draw_index};
                if (from_frame == ANIM_PUNCH_R_1)
                    memcpy(&quads[k], &quads[32*p+1 + 3 + 3 + 1], 2*sizeof(struct quad));
                else
                {
                    memcpy(&quads[k], &quads[32*p+1 + 3 + 1], 2*sizeof(struct quad));
                    dy *= -1;
                }
                for (int dk=0; dk<2; ++dk)
                {
                    quads[k+dk].x += 1.0 - dy/2;
                    quads[k+dk].ix += 1 - dy/2;
                    quads[k+dk].y += dy;
                    quads[k+dk].iy += dy;
                    quads[k+dk].draw_index = draw_indices[dk];
                    quads[k+dk].lifetime = -8.5;
                    //quads[k+dk].vx = players[p].vx + 128.0/dt;
                    //quads[k+dk].vy = players[p].vy;
                    //quads[k+dk].vz = players[p].vz;
                }
                draw_add_projectile(k, k+1);
            }
            else
            {
                players[p].punch_charge += 0.5 + dt;
                animation_interrupt(p, (ANIM_PUNCH_R_0 + 2 - ((from_frame%4)/2)*2) | next_flipped);
            }
            return;
        }
        else
        {
            if (next_flipped)
            {
                if (GAMEPAD_PRESSED(p, right))
                    next_flipped = 0;
            }
            else if (GAMEPAD_PRESSED(p, left))
                next_flipped = ANIM_FACE_LEFT;
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
        animation_interrupt(p, (ANIM_PUNCH_R_1 + next_frame) | next_flipped);
        frame_rate = 256*players[p].punch_charge;
        if (players[p].punch_charge > 16)
        {
            message("firing off a punch\n");
            int k=32*p + 16;
            if (quads[k].draw_index)
            {
                ++k;
                if (quads[k].draw_index)
                    goto punch_finalize;
            }
            memcpy(&quads[k], &quads[32*p+3 + 3*(next_frame/2) + 2], sizeof(struct quad));
            int width = 4+players[p].punch_charge/4;
            quads[k].draw_index = 0;
            quads[k].x += 3;
            quads[k].y -= width/2;
            quads[k].height = width;
            quads[k].width = width;
            if (!next_flipped)
                quads[k].vx += width;
            else
                quads[k].vx -= width;
            quads[k].lifetime = players[p].punch_charge;
            quads[k].color = RGB(130,130,130);
            quads[k].edge_color = RGB(250,250,250);
            draw_add_projectile(k, k);
        }
        punch_finalize:
        players[p].punch_charge = 0;
        next_frame = ANIM_PUNCH_R_0 + next_frame;
    }
    else if (GAMEPAD_PRESSED(p, A))
    {
        players[p].jump_charge = 0;
        if (players[p].kick_charge == 0)
        {
            players[p].kick_charge += 0.5 + dt;
            animation_interrupt(p, (ANIM_KICK_R_0 + 2 - ((from_frame%4)/2)*2) | next_flipped);
            return;
        }
        else
        {
            players[p].kick_charge += 0.5 + dt;
            if (players[p].kick_charge > 256)
            {
                players[p].kick_charge = 256;
                goto kick_now;
            }
            next_frame = to_frame;
        }
    }
    else if (players[p].kick_charge)
    {
        kick_now:
        next_frame = ((next_frame%4)/2)*2;
        animation_interrupt(p, (ANIM_KICK_R_1 + next_frame)|next_flipped);
        frame_rate = 8*players[p].kick_charge;
        players[p].kick_charge = 0;
        next_frame = ANIM_KICK_R_0 + next_frame;
    }
    else if (GAMEPAD_PRESSED(p, right))
    {
        players[p].vx += 1 + 0.05*players[p].run_charge;
        if (players[p].vx > 0)
            next_flipped = 0;
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
        if (players[p].vx < 0)
            next_flipped = ANIM_FACE_LEFT;
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
    
    animation_tween(p, dt, frame_rate, next_frame | next_flipped);
}

void opm_air(int p, float dt)
{
    if (vga_frame % 32 == 0)
        message("fly %f at vx=%f\n", players[p].run_charge, players[p].vx);
    if (opm_run_charge(p, dt) && !GAMEPAD_PRESSED(p, dpad))
        physics_slow_time(players[p].run_charge);

    int to_frame = ANIM_TO(p);
    int to_flipped = to_frame & ANIM_FACE_LEFT;
    to_frame &= ~ANIM_FACE_LEFT;
    if (to_frame/2 == ANIM_FLY_0/2)
        animation_tween(p, dt, 0.1, ANIM_FLY_0 | (1-(to_frame&1)) | to_flipped);
    else
        animation_tween(p, dt, 0.1, ANIM_FLY_0 | to_flipped);
}

static inline void opm_punch_wind(int k, float dt)
{
    quads[k].y -= dt;
    quads[k].x -= dt;
    int inv_dt = 1.0/dt;
    if (vga_frame % inv_dt == 0 && quads[k].width < 254)
    {
        quads[k].width += 2;
        quads[k].height += 2;
    }
}

float sqr(float x)
{
    return x*x;
}

void opm_cape(int k, float x0, float y0, float z0, float *px1, float *pz1, float dt)
{
    // simulate gravity on cape:
    *pz1 += gravity * dt;
    float dx = *px1 - x0;
    float dz = *pz1 - z0;
    float mag_squared = sqr(dx) + sqr(dz);
    if (mag_squared > CAPE_LENGTH*CAPE_LENGTH)
    {
        if (fabs(dx) < 0.75 && dz > 0 && fabs(dz - CAPE_LENGTH) < 0.75)
            // cancel gravity force if cape (px1, pz1) is below its anchor point (x0, z0)
            *pz1 -= gravity * dt;
        else if (mag_squared > (CAPE_LENGTH+4.0f)*(CAPE_LENGTH+4.0f))
        {
            // cape is too stretched, renormalize the vector and pull it in (gradually):
            float mag = sqrt(mag_squared);
            float norm = (CAPE_LENGTH*dt + 0.25f*mag)/(0.25f+dt)/mag;
            *px1 = x0 + dx*norm;
            *pz1 = z0 + dz*norm;
        }
        else
        {
            // cape is slightly stretched, use spring force.
            *px1 -= 0.1*dx*dt;
            *pz1 -= 0.1*dz*dt;
        }
        // bottom of cape should have a back effect on mid cape, ignoring third law for now.
    }
  
    // one of these gets recalculated twice, i think it's ok...
    int ix0 = round(x0-map_offset_x);
    int ix1 = round(*px1-map_offset_x);
    int iy0 = STREET_LEVEL_Y + (int)round(y0+z0);
    int iy1 = STREET_LEVEL_Y + (int)round(y0+*pz1);
    draw_setup_quad2(k, ix0, iy0, ix1, iy1, 16, 4);
}

void opm_projectile(int p, float dt)
{
    int k=32*p+16;
    if (quads[k].draw_index)
        opm_punch_wind(k, dt);
    if (quads[++k].draw_index)
        opm_punch_wind(k, dt);
    ++k;

    float x0 = quads[k-16].x + quads[k-16].width/2;
    float y0 = quads[k-16].y;
    float z0 = quads[k-16].z;
    float *px1 = &quads[k].x;
    float *pz1 = &quads[k].z;
    quads[k+1].y = quads[k].y = y0 - 0.1;
    float *px2 = &quads[k+1].x;
    float *pz2 = &quads[k+1].z;

    dt += 0.1;
    opm_cape(k, x0, y0, z0, px1, pz1, dt);
    opm_cape(k+1, *px1, y0, *pz1, px2, pz2, dt);
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
