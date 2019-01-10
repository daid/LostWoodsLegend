
function walk4dir()
    if irandom(0, 100) < 50 then
        if irandom(0, 100) < 50 then
            walk_direction = Vector2(1, 0)
        else
            walk_direction = Vector2(-1, 0)
        end
    else
        if irandom(0, 100) < 50 then
            walk_direction = Vector2(0, 1)
        else
            walk_direction = Vector2(0, -1)
        end
    end
    for n=0,irandom(50, 100) do
        this.setPosition(this.getPosition() + walk_direction * walk_speed / 60.0);
        yield()
        if touch_wall then break end
    end
    if projectile_speed ~= nil and irandom(0, 100) < 50 then
        this.createProjectile(walk_direction * projectile_speed / 60.0, projectile_sprite);
        for n=0, fire_delay do
            yield()
        end
    end
end

function walk8dir()
    if walk_direction == nil then walk_direction = Vector2(1, 0) end
    if irandom(0, 100) < 50 then
        walk_direction = walk_direction:rotate(45)
    else
        walk_direction = walk_direction:rotate(-45)
    end

    for n=0,irandom(50, 100) do
        this.setPosition(this.getPosition() + walk_direction * walk_speed / 60.0);
        yield()
        if touch_wall then break end
    end
    if projectile_speed ~= nil and irandom(0, 100) < 50 then
        this.createProjectile(walk_direction * projectile_speed / 60.0, projectile_sprite);
        for n=0, fire_delay do
            yield()
        end
    end
end

function updateAnimation()
    if walk_direction == nil then walk_direction = Vector2(0, 0) end
    if walk_direction.x < 0 then
        this.animation("LEFT")
    elseif walk_direction.x > 0 then
        this.animation("RIGHT")
    elseif walk_direction.y < 0 then
        this.animation("DOWN")
    elseif walk_direction.y > 0 then
        this.animation("UP")
    end
end

projectile_speed = tonumber(projectile_speed)
update = _ENV[movement]
update = update or walk4dir
