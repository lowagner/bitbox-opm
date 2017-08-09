void henchman_start_level(int p)
{
    players[p].vz = 0;
    players[p].vx = 5;
}

static const uint8_t henchman_next_frames[ANIM_COUNT] = {
    [ANIM_IDLE_R]=ANIM_IDLE_L,
    [ANIM_IDLE_L]=ANIM_IDLE_R,
    [ANIM_GUARD_R]=ANIM_IDLE_R,
    [ANIM_GUARD_L]=ANIM_IDLE_L,
    [ANIM_WALK_R_0]=ANIM_WALK_R_1,
    [ANIM_WALK_L_0]=ANIM_WALK_L_1,
    [ANIM_WALK_R_1]=ANIM_WALK_L_0,
    [ANIM_WALK_L_1]=ANIM_WALK_R_0,
    [ANIM_RUN_R_0]=ANIM_WALK_R_1,
    [ANIM_RUN_L_0]=ANIM_WALK_L_1,
    [ANIM_RUN_R_1]=ANIM_WALK_L_0,
    [ANIM_RUN_L_1]=ANIM_WALK_R_0,
    [ANIM_CROUCH_R]=ANIM_IDLE_R,
    [ANIM_CROUCH_L]=ANIM_IDLE_L,
    [ANIM_FLY_R]=ANIM_FLY_L,
    [ANIM_FLY_L]=ANIM_FLY_R,
    [ANIM_PUNCH_R_0]=ANIM_PUNCH_R_1,
    [ANIM_PUNCH_L_0]=ANIM_PUNCH_L_1,
    [ANIM_PUNCH_R_1]=ANIM_WALK_R_0,
    [ANIM_PUNCH_L_1]=ANIM_WALK_L_0,
    [ANIM_KICK_R_0]=ANIM_KICK_R_1,
    [ANIM_KICK_L_0]=ANIM_KICK_L_1,
    [ANIM_KICK_R_1]=ANIM_IDLE_R,
    [ANIM_KICK_L_1]=ANIM_IDLE_L,
    [ANIM_GRAB]=ANIM_IDLE_L,
    [ANIM_GRAB_THROW]=ANIM_IDLE_L,
    [ANIM_GRAB_PUNCH]=ANIM_GRAB,
    [ANIM_GRAB_KICK]=ANIM_GRAB,
    [ANIM_USER_R]=ANIM_IDLE_R,
    [ANIM_USER_L]=ANIM_IDLE_L,
    [ANIM_HURT]=ANIM_IDLE_R,
    [ANIM_BAD_HURT]=ANIM_HURT,
};

static const float henchman_frame_rates[ANIM_COUNT] = {
    [ANIM_IDLE_R]=0.125,
    [ANIM_IDLE_L]=0.125,
    [ANIM_GUARD_R]=0.5,
    [ANIM_GUARD_L]=0.5,
    [ANIM_WALK_R_0]=0.2,
    [ANIM_WALK_L_0]=0.2,
    [ANIM_WALK_R_1]=0.2,
    [ANIM_WALK_L_1]=0.2,
    [ANIM_RUN_R_0]=0.4,
    [ANIM_RUN_L_0]=0.4,
    [ANIM_RUN_R_1]=0.4,
    [ANIM_RUN_L_1]=0.4,
    [ANIM_CROUCH_R]=0.5,
    [ANIM_CROUCH_L]=0.5,
    [ANIM_FLY_R]=0.2,
    [ANIM_FLY_L]=0.2,
    [ANIM_PUNCH_R_0]=0.5,
    [ANIM_PUNCH_L_0]=0.5,
    [ANIM_PUNCH_R_1]=0.3,
    [ANIM_PUNCH_L_1]=0.3,
    [ANIM_KICK_R_0]=0.5,
    [ANIM_KICK_L_0]=0.2,
    [ANIM_KICK_R_1]=0.3,
    [ANIM_KICK_L_1]=0.3,
    [ANIM_GRAB]=0.5,
    [ANIM_GRAB_THROW]=0.5,
    [ANIM_GRAB_PUNCH]=0.5,
    [ANIM_GRAB_KICK]=0.5,
    [ANIM_USER_R]=0.1,
    [ANIM_USER_L]=0.1,
    [ANIM_HURT]=0.5,
    [ANIM_BAD_HURT]=0.5,
};

void henchman_ground(int p, float dt)
{
    int from_frame = ANIM_FROM(p);
    //int from_flipped = from_frame & ANIM_FACE_LEFT;
    from_frame &= ~ANIM_FACE_LEFT;
    int to_frame = ANIM_TO(p);
    int to_flipped = to_frame & ANIM_FACE_LEFT;
    to_frame &= ~ANIM_FACE_LEFT;
    int next_frame = henchman_next_frames[to_frame];
    int next_flipped = to_flipped;

    static float frame_rate = 0.25;
    if (to_frame/4 != ANIM_PUNCH_R_0/4 || (to_frame/2) % 2 == 0)
        frame_rate = henchman_frame_rates[to_frame];

    if (gamepad_PRESSED(p, Y))
    {
        players[p].run_charge += dt;
        if (players[p].run_charge > 64)
            players[p].run_charge = 64;
    }
    else
        players[p].run_charge -= 0.5*players[p].run_charge * dt;

    if (gamepad_PRESSED(p, L))
    {
        if (gamepad_PRESSED(p, B))
        {
            // uppercut
            if (from_frame/2 == ANIM_USER_R/2)
                goto after_movement;
            next_frame = ANIM_USER_R + from_frame%2;
            animation_interrupt(p, next_frame | next_flipped);
            frame_rate = 0.5 + players[p].jump_charge/32;
            players[p].jump_charge = 0;
            goto after_movement;
        }
        if (players[p].jump_charge == 0)
        {
            players[p].jump_charge += dt;
            animation_interrupt(p, ANIM_CROUCH_R | (next_frame&1) | next_flipped);
            goto pre_jump;
        }
        players[p].jump_charge += dt;
        if (players[p].jump_charge >= 32)
            players[p].jump_charge = 32;
        if (gamepad_PRESS(p, dpad))
        {
            if (gamepad_PRESSED(p, left))
                players[p].vx -= players[p].jump_charge/8;
            else if (gamepad_PRESSED(p, right))
                players[p].vx += players[p].jump_charge/8;
            if (gamepad_PRESSED(p, up))
                players[p].vy -= players[p].jump_charge/16;
            else if (gamepad_PRESSED(p, down))
                players[p].vy += players[p].jump_charge/16;
            
            goto jump_now;
        }
        pre_jump:
        next_frame = ANIM_CROUCH_R | (next_frame&1);
        frame_rate = henchman_frame_rates[ANIM_CROUCH_R];
        players[p].vx -= 0.5 * players[p].vx * dt;
        players[p].vy -= 0.5 * players[p].vy * dt;
        goto after_movement;
    }
    else if (players[p].jump_charge)
    {
        jump_now:
        message("jump %f\n", players[p].jump_charge);
        players[p].vx *= 2.0;
        players[p].vy *= 2.0;
        players[p].vz = -1.0 -players[p].jump_charge/4;
        players[p].jump_charge = 0;
        animation_interrupt(p, ANIM_FLY_R | next_flipped);
        frame_rate = henchman_frame_rates[ANIM_FLY_R];
        next_frame = ANIM_FLY_L;
        // the above line does nothing if we return...
        return;
    }
    
    players[p].vx -= 0.5 * players[p].vx * dt;
    players[p].vy -= 0.5 * players[p].vy * dt;
    
    if (gamepad_PRESSED(p, B))
    {
        if (players[p].punch_charge == 0)
        {
            if (from_frame/2 == ANIM_USER_R/2)
            {
                players[p].jump_charge = 0;
                goto after_movement;
            }
            players[p].jump_charge = 0;
            players[p].punch_charge += dt;
            animation_interrupt(p, (ANIM_PUNCH_R_0 + 1 - (from_frame&1)) | next_flipped);
            return;
        }
        else
        {
            players[p].jump_charge = 0;
            players[p].punch_charge += dt;
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
        ALLOW_TURN(p);
        next_frame = next_frame&1;
        animation_interrupt(p, ANIM_PUNCH_R_1 | next_frame | next_flipped);
        frame_rate = 0.5 + players[p].punch_charge/256;
        players[p].punch_charge = 0;
        next_frame = ANIM_PUNCH_R_0 + next_frame;
    }
    else if (gamepad_PRESSED(p, A))
    {
        players[p].jump_charge = 0;
        if (players[p].kick_charge == 0)
        {
            players[p].kick_charge += dt;
            animation_interrupt(p, (ANIM_KICK_R_0 + 1 - (from_frame&1)) | next_flipped);
            return;
        }
        else
        {
            players[p].kick_charge += dt;
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
        next_frame = next_frame&1;
        animation_interrupt(p, (ANIM_KICK_R_1 + next_frame)|next_flipped);
        frame_rate = 0.5+players[p].kick_charge/256;
        players[p].kick_charge = 0;
        next_frame = ANIM_KICK_R_0 + next_frame;
    }
    else if (gamepad_PRESSED(p, X))
    {
        frame_rate = 0.5;
        next_frame = ANIM_GUARD_R + (to_frame%2);
        ALLOW_TURN(p);
    }
    else if (gamepad_PRESSED(p, right))
    {
        players[p].vx += (0.5 + 0.04*players[p].run_charge)*dt;
        if (players[p].vx > 0)
            next_flipped = 0;
        if (players[p].vx > 6.0)
            next_frame = ANIM_RUN_R_0 + next_frame%4;
        else
        {
            frame_rate *= 0.5 + fabs(players[p].vx)/12.0;
            next_frame = ANIM_WALK_R_0 + next_frame%4;
        }
    }
    else if (gamepad_PRESSED(p, left))
    {
        players[p].vx -= (0.5 + 0.04*players[p].run_charge)*dt;
        if (players[p].vx < 0)
            next_flipped = ANIM_FACE_LEFT;
        if (players[p].vx < -6.0)
            next_frame = ANIM_RUN_R_0 + next_frame%4;
        else
        {
            frame_rate *= 0.5 + fabs(players[p].vx)/12.0;
            next_frame = ANIM_WALK_R_0 + next_frame%4;
        }
    }
    else if (!(gamepad_old[p] & (gamepad_up | gamepad_down)))
    {
        if (next_frame/4 == ANIM_WALK_R_0/4)
        {
            if (players[p].vx < 2.0)
                next_frame %= 2; 
        }
        else if (next_frame/4 == ANIM_RUN_R_0/4)
        {
            if (players[p].vx < 6.0)
                next_frame -= 4;
        }
        goto after_movement;
    }
    if (gamepad_PRESSED(p, up))
    {
        players[p].vy -= (0.5 + 0.03*players[p].run_charge)*dt;
        if (!(gamepad_old[p] & (gamepad_left | gamepad_right)))
        {
            if (fabs(players[p].vx) > 6.0)
                next_frame = ANIM_RUN_R_0 + next_frame%4;
            else
                next_frame = ANIM_WALK_R_0 + next_frame%4;
        }
    }
    else if (gamepad_PRESSED(p, down))
    {
        players[p].vy += (0.5 + 0.03*players[p].run_charge)*dt;
        if (!(gamepad_old[p] & (gamepad_left | gamepad_right)))
        {
            if (fabs(players[p].vx) > 6.0)
                next_frame = ANIM_RUN_R_0 + next_frame%4;
            else
                next_frame = ANIM_WALK_R_0 + next_frame%4;
        }
    }

    after_movement:
    animation_tween(p, dt, frame_rate, next_frame | next_flipped);
}

void henchman_air(int p, float dt)
{
    int from_frame = ANIM_FROM(p);
    //int from_flipped = from_frame & ANIM_FACE_LEFT;
    from_frame &= ~ANIM_FACE_LEFT;
    int to_frame = ANIM_TO(p);
    int to_flipped = to_frame & ANIM_FACE_LEFT;
    to_frame &= ~ANIM_FACE_LEFT;
    int next_flipped = to_flipped;
    int next_frame = ANIM_FLY_R | (1-(to_frame&1));
    
    static float frame_rate = 0.1;
    if (to_frame/4 != ANIM_PUNCH_R_0/4 || to_frame % 2 == 0)
        frame_rate = henchman_frame_rates[to_frame];
    
    animation_tween(p, dt, frame_rate, next_frame | next_flipped);
}

void henchman_projectile(int p, float dt)
{
}

void henchman_AI(int p, float dt)
{
    players[p].AI.reaction += dt;
    switch (players[p].AI.action)
    {
        case Wandering:
            if (players[p].AI.reaction > 8)
            {
                players[p].AI.reaction = 0;
                players[p].AI.action = rand()%Closing;
                return;
            }
            if (rand()&1)
                players[p].AI.direction = (players[p].AI.direction+1)%8;
            else
                players[p].AI.direction = (players[p].AI.direction+7)%8;
            AI_set_direction(p);
            return;
        case Thinking:
            gamepad_AI[p] = 0;
            if (players[p].AI.reaction > 2)
            {
                players[p].AI.reaction = 0;
                players[p].AI.action = Looking;
                return;
            }
            return;
        case Looking:
            if (players[p].AI.reaction < 4)
                return;
            players[p].AI.reaction = 0;
            if (players[0].health > 0)
            {
                if (players[1].health > 0)
                    players[p].AI.target_p = player_distance2(p, 0) < player_distance2(p, 1) ? 0 : 1;
                else
                    players[p].AI.target_p = 0;
            }
            else if (players[1].health > 0)
                players[p].AI.target_p = 1;
            else
            {
                players[p].AI.action = Wandering;
                return;
            }
            players[p].AI.action = Closing;
            AI_move_to_player(p, players[p].AI.target_p, 6.0);
            return;
        case Closing:
            if (players[p].AI.reaction > 4)
            {
                int tp = players[p].AI.target_p;
                players[p].AI.reaction = 0;
                if (player_distance2(p, tp) > 64.0)
                    AI_move_to_player(p, tp, 6.0);
                else
                    players[p].AI.action = DecidingAttack;
                return;
            }
            return;
        case DecidingAttack:
            if (players[p].AI.reaction > 2)
            {
                players[p].AI.reaction = 0;
                players[p].AI.action = Attacking;
                return;
            }
            return;
        case Attacking:
            if (players[p].AI.reaction > 3)
            {
                players[p].AI.reaction = 0;
                players[p].AI.action = Looking;
                return;
            }
            return;
        case Circling:
            if (players[p].AI.reaction > 8)
            {
                players[p].AI.reaction = 0;
                players[p].AI.action = Closing;
                return;
            }
            return;
        case Special:
            if (players[p].AI.reaction > 2)
            {
                players[p].AI.reaction = 0;
                players[p].AI.action = Looking;
                return;
            }
            return;
    }
}
