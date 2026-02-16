#include "EduSandbox.h"

// Helper function to draw filled circle
void DrawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

// Helper function to draw circle outline
void DrawCircleOutline(SDL_Renderer* renderer, int cx, int cy, int radius, int thickness = 2) {
    for (int t = 0; t < thickness; t++) {
        int r = radius - t;
        for (int angle = 0; angle < 360; angle++) {
            float rad = angle * M_PI / 180.0f;
            int x = cx + (int)(r * cos(rad));
            int y = cy + (int)(r * sin(rad));
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

// ===== Ball Implementation =====

Ball::Ball(const Vector2& pos, float radius)
    : Entity(EntityType::BALL, pos), radius(radius) {
    color = Color::Red();
    physics.restitution = 0.75f;
    physics.mass = radius / 25.0f;
    physics.friction = 0.05f; // low rolling friction for balls
    name = "Ball";
    updateBoundingBox();
}

void Ball::setRadius(float r) {
    radius = r;
    physics.mass = r / 25.0f;
    updateBoundingBox();
}

void Ball::updateBoundingBox() {
    boundingBox = AABB(
        Vector2(position.x - radius, position.y - radius),
        Vector2(position.x + radius, position.y + radius)
    );
}

void Ball::render(SDL_Renderer* renderer) {
    // Draw shadow
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 30);
    DrawFilledCircle(renderer, (int)position.x + 3, (int)position.y + 3, (int)radius);
    
    // Draw main circle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    DrawFilledCircle(renderer, (int)position.x, (int)position.y, (int)radius);
    
    // Draw highlight
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    DrawFilledCircle(renderer, (int)position.x - radius/3, (int)position.y - radius/3, (int)(radius/3));
    
    // Draw selection outline
    if (selected || dragging) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
        DrawCircleOutline(renderer, (int)position.x, (int)position.y, (int)radius + 3, 3);
    }
}

std::string Ball::getDescription() const {
    return "Ball (r=" + std::to_string((int)radius) + ")";
}

// ===== Block Implementation =====

Block::Block(const Vector2& pos, float width, float height)
    : Entity(EntityType::BLOCK, pos), width(width), height(height) {
    color = Color::Blue();
    physics.restitution = 0.4f;
    physics.mass = (width * height) / 3000.0f;
    physics.friction = 0.45f; // more surface friction for blocks
    name = "Block";
    updateBoundingBox();
}

void Block::setSize(float w, float h) {
    width = w;
    height = h;
    physics.mass = (w * h) / 3000.0f;
    updateBoundingBox();
}

void Block::updateBoundingBox() {
    boundingBox = AABB(
        Vector2(position.x - width/2, position.y - height/2),
        Vector2(position.x + width/2, position.y + height/2)
    );
}

void Block::render(SDL_Renderer* renderer) {
    SDL_Rect shadowRect = {
        (int)(position.x - width/2 + 3),
        (int)(position.y - height/2 + 3),
        (int)width,
        (int)height
    };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 30);
    SDL_RenderFillRect(renderer, &shadowRect);
    
    SDL_Rect rect = {
        (int)(position.x - width/2),
        (int)(position.y - height/2),
        (int)width,
        (int)height
    };
    
    // Main rectangle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    
    // Border
    SDL_SetRenderDrawColor(renderer, 
        (Uint8)(color.r * 0.7), 
        (Uint8)(color.g * 0.7), 
        (Uint8)(color.b * 0.7), 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    // Highlight
    SDL_Rect highlight = {rect.x + 5, rect.y + 5, (int)(width/3), (int)(height/3)};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
    SDL_RenderFillRect(renderer, &highlight);
    
    // Selection outline
    if (selected || dragging) {
        SDL_Rect selectRect = {rect.x - 3, rect.y - 3, rect.w + 6, rect.h + 6};
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
        for (int i = 0; i < 3; i++) {
            SDL_RenderDrawRect(renderer, &selectRect);
            selectRect.x--; selectRect.y--;
            selectRect.w += 2; selectRect.h += 2;
        }
    }
}

std::string Block::getDescription() const {
    return "Block (" + std::to_string((int)width) + "x" + std::to_string((int)height) + ")";
}

// ===== Ground Implementation =====

Ground::Ground(const Vector2& pos, float width, float thickness)
    : Entity(EntityType::GROUND, pos), width(width), thickness(thickness) {
    color = Color::Brown();
    physics.type = PhysicsType::STATIC;
    physics.restitution = 0.5f;
    physics.friction = 0.9f; // sticky ground
    name = "Ground";
    updateBoundingBox();
}

void Ground::updateBoundingBox() {
    boundingBox = AABB(
        Vector2(position.x - width/2, position.y - thickness/2),
        Vector2(position.x + width/2, position.y + thickness/2)
    );
}

void Ground::render(SDL_Renderer* renderer) {
    SDL_Rect rect = {
        (int)(position.x - width/2),
        (int)(position.y - thickness/2),
        (int)width,
        (int)thickness
    };
    
    // Main ground
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    
    // Top highlight
    SDL_Rect topLine = {rect.x, rect.y, rect.w, 3};
    SDL_SetRenderDrawColor(renderer, 
        (Uint8)std::min(255, color.r + 40),
        (Uint8)std::min(255, color.g + 40),
        (Uint8)std::min(255, color.b + 40), 255);
    SDL_RenderFillRect(renderer, &topLine);
    
    // Grass pattern
    SDL_SetRenderDrawColor(renderer, 60, 150, 60, 255);
    for (int x = rect.x; x < rect.x + rect.w; x += 10) {
        SDL_RenderDrawLine(renderer, x, rect.y, x, rect.y - 5);
        SDL_RenderDrawLine(renderer, x + 3, rect.y, x + 3, rect.y - 3);
    }
    
    if (selected || dragging) {
        SDL_Rect selectRect = {rect.x - 3, rect.y - 3, rect.w + 6, rect.h + 6};
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
        SDL_RenderDrawRect(renderer, &selectRect);
    }
}

std::string Ground::getDescription() const {
    return "Ground (" + std::to_string((int)width) + " wide)";
}

// ===== Ramp Implementation =====

Ramp::Ramp(const Vector2& pos, float width, float height)
    : Entity(EntityType::RAMP, pos), width(width), height(height) {
    color = Color::Gray();
    physics.type = PhysicsType::STATIC;
    physics.restitution = 0.3f;
    physics.friction = 0.6f; // ramp surface friction
    angle = atan2(height, width);
    name = "Ramp";
    updateBoundingBox();
}

void Ramp::updateBoundingBox() {
    boundingBox = AABB(
        Vector2(position.x, position.y),
        Vector2(position.x + width, position.y + height)
    );
}

void Ramp::render(SDL_Renderer* renderer) {
    // Triangle points
    Sint16 vx[] = {
        (Sint16)position.x,
        (Sint16)(position.x + width),
        (Sint16)position.x
    };
    Sint16 vy[] = {
        (Sint16)position.y,
        (Sint16)(position.y + height),
        (Sint16)(position.y + height)
    };
    
    // Fill triangle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int y = (int)position.y; y <= (int)(position.y + height); y++) {
        float t = (y - position.y) / height;
        int x1 = (int)position.x;
        int x2 = (int)(position.x + width * t);
        SDL_RenderDrawLine(renderer, x1, y, x2, y);
    }
    
    // Outline
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    for (int i = 0; i < 3; i++) {
        SDL_RenderDrawLine(renderer, vx[i], vy[i], vx[(i+1)%3], vy[(i+1)%3]);
    }
    
    if (selected || dragging) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
        for (int t = 0; t < 3; t++) {
            for (int i = 0; i < 3; i++) {
                SDL_RenderDrawLine(renderer, vx[i]-t, vy[i]-t, vx[(i+1)%3]-t, vy[(i+1)%3]-t);
            }
        }
    }
}

std::string Ramp::getDescription() const {
    return "Ramp (" + std::to_string((int)width) + "x" + std::to_string((int)height) + ")";
}

// ===== Arrow Implementation =====

Arrow::Arrow(const Vector2& pos, const Vector2& dir, const std::string& label)
    : Entity(EntityType::ARROW, pos), direction(dir.normalized()), 
      length(80.0f), label(label) {
    color = Color::Green();
    physics.type = PhysicsType::KINEMATIC;
    name = "Arrow";
    updateBoundingBox();
}

void Arrow::setDirection(const Vector2& dir) {
    direction = dir.normalized();
    updateBoundingBox();
}

void Arrow::updateBoundingBox() {
    Vector2 end = position + (direction * length);
    boundingBox = AABB(
        Vector2(std::min(position.x, end.x) - 10, std::min(position.y, end.y) - 10),
        Vector2(std::max(position.x, end.x) + 10, std::max(position.y, end.y) + 10)
    );
}

void Arrow::render(SDL_Renderer* renderer) {
    Vector2 end = position + (direction * length);
    
    // Arrow shaft
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int i = -2; i <= 2; i++) {
        Vector2 perp(-direction.y * i, direction.x * i);
        SDL_RenderDrawLine(renderer, 
            (int)(position.x + perp.x), (int)(position.y + perp.y),
            (int)(end.x + perp.x), (int)(end.y + perp.y));
    }
    
    // Arrow head
    Vector2 perpDir(-direction.y, direction.x);
    Vector2 head1 = end - (direction * 15) + (perpDir * 10);
    Vector2 head2 = end - (direction * 15) - (perpDir * 10);
    
    SDL_RenderDrawLine(renderer, (int)end.x, (int)end.y, (int)head1.x, (int)head1.y);
    SDL_RenderDrawLine(renderer, (int)end.x, (int)end.y, (int)head2.x, (int)head2.y);
    SDL_RenderDrawLine(renderer, (int)head1.x, (int)head1.y, (int)head2.x, (int)head2.y);
}

std::string Arrow::getDescription() const {
    return "Arrow: " + label;
}

// ===== TextLabel Implementation =====

TextLabel::TextLabel(const Vector2& pos, const std::string& text)
    : Entity(EntityType::TEXT_LABEL, pos), text(text), fontSize(18) {
    color = Color::Black();
    physics.type = PhysicsType::KINEMATIC;
    name = "Label";
    updateBoundingBox();
}

void TextLabel::updateBoundingBox() {
    float width = text.length() * fontSize * 0.6f;
    boundingBox = AABB(position, Vector2(position.x + width, position.y + fontSize));
}

void TextLabel::render(SDL_Renderer* renderer) {
    // Text rendering will be handled by the renderer with TTF
    if (selected || dragging) {
        SDL_Rect selectRect = {
            (int)boundingBox.min.x - 3,
            (int)boundingBox.min.y - 3,
            (int)(boundingBox.max.x - boundingBox.min.x) + 6,
            (int)(boundingBox.max.y - boundingBox.min.y) + 6
        };
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 100);
        SDL_RenderFillRect(renderer, &selectRect);
    }
}

std::string TextLabel::getDescription() const {
    return "Label: \"" + text + "\"";
}
