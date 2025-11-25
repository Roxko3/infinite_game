#include "test_scene.h"

void thread_func(void *p_user_data) {
    Chunk *c = reinterpret_cast<Chunk*>(p_user_data);
    c->generate();
    c->_is_ready.set();
}

void TestScene::input_event(const Ref<InputEvent> &event){
    Ref<InputEventKey> iek = event;

      if(iek.is_valid()) {
        if(iek->is_echo())
            return;

        /*if(!iek->is_pressed())
            return;*/

        uint32_t scancode = iek->get_physical_scancode();
        bool pressed = iek->is_pressed();

        if(scancode == KEY_W){
            up = pressed;
        }
        else if(scancode == KEY_S){
            down = pressed;
        }
        else if(scancode == KEY_A){
            left = pressed;
        }
        else if(scancode == KEY_D){
            right = pressed;
        }

        return;
    }

}
void TestScene::update(float delta){
    _manage_threads();

    Vector2 direction = Vector2(0, 0);

    if (up)    direction.y -= 1;
    if (down)  direction.y += 1;
    if (right) direction.x += 1;
    if (left)  direction.x -= 1;

    if (direction != Vector2(0, 0)) {
        direction.normalize();
    }

    Vector2 desired = direction * _player_speed * delta;

    Vector2 try_x = _player_pos + Vector2(desired.x, 0);

    float half = _player_size * 0.5f;

    bool blocked_x =
        is_solid_tile(try_x.x - half, _player_pos.y - half) ||
        is_solid_tile(try_x.x + half, _player_pos.y - half) ||
        is_solid_tile(try_x.x - half, _player_pos.y + half) ||
        is_solid_tile(try_x.x + half, _player_pos.y + half);

    if(!blocked_x) {
        _player_pos.x = try_x.x;
    }

    Vector2 try_y = _player_pos + Vector2(0, desired.y);

    bool blocked_y =
        is_solid_tile(_player_pos.x - half, try_y.y - half) ||
        is_solid_tile(_player_pos.x + half, try_y.y - half) ||
        is_solid_tile(_player_pos.x - half, try_y.y + half) ||
        is_solid_tile(_player_pos.x + half, try_y.y + half);

    if(!blocked_y) {
        _player_pos.y = try_y.y;
    }

    check_diamond_pickup();

    int player_chunk_x = floor(_player_pos.x / (_tile_size * _chunk_size));
    int player_chunk_y = floor(_player_pos.y / (_tile_size * _chunk_size));

    for (int y = -_view_radius; y <= _view_radius; y++) {
        for (int x = -_view_radius; x <= _view_radius; x++) {
            ensure_chunk(player_chunk_x + x, player_chunk_y + y);
        }
    }

}
void TestScene::render(){
    Renderer *r = Renderer::get_singleton();

    r->clear_screen(Color(1, 1, 1));
    r->camera_2d_projection_set_to_window();

    Vector2 cam(
    r->get_window_size().width * 0.5f - _player_pos.x,
    r->get_window_size().height * 0.5f - _player_pos.y
);

    int player_chunk_x = floor(_player_pos.x / (_tile_size * _chunk_size));
    int player_chunk_y = floor(_player_pos.y / (_tile_size * _chunk_size));

    for (int y=-_view_radius; y<=_view_radius; y++) {
        for (int x=-_view_radius; x<=_view_radius; x++) {

            int cx = player_chunk_x + x;
            int cy = player_chunk_y + y;

            String key = make_key(cx, cy);

            if (!_chunks.has(key))
                continue;

            Chunk& c = *_chunks[key];

            if (!c._is_ready.is_set())
                continue;

            int world_x = cx * _chunk_size * _tile_size;
            int world_y = cy * _chunk_size * _tile_size;

            for (int y=0; y<_chunk_size; y++) {
                for (int x=0; x<_chunk_size; x++) {

                    int tile = c.get(x,y);

                    switch(tile) {
                case 0:
                        r->draw_texture(_grass_texture, Rect2(cam.x + world_x + x * _tile_size, cam.y + world_y + y * _tile_size, _tile_size, _tile_size));
                    break;
                case 1:
                    r->draw_texture(_wall_texture, Rect2(cam.x + world_x + x * _tile_size, cam.y + world_y + y * _tile_size, _tile_size, _tile_size));
                    break;
                case 2:
                        r->draw_texture(_grass_texture, Rect2(cam.x + world_x + x * _tile_size, cam.y + world_y + y * _tile_size, _tile_size, _tile_size));
                        r->draw_texture(_diamond_texture, Rect2(cam.x + world_x + x * _tile_size, cam.y + world_y + y * _tile_size, _tile_size, _tile_size));
                    break;
                default:
                        r->draw_texture(_grass_texture, Rect2(cam.x + world_x + x * _tile_size, cam.y + world_y + y * _tile_size, _tile_size, _tile_size));
                    break;
                    }

                }
            }
        }
    }

    float half = _player_size * 0.5f;
    r->draw_texture(_player_texture,Rect2(cam.x + _player_pos.x - half,cam.y + _player_pos.y - half,_player_size,_player_size));
    r->draw_text_2d("Points: " + String::num(_points), _font, Vector2(5,5), Color(1,0,0));

    GUI::new_frame();
    ImGui::Begin("Menu");
    if (ImGui::Button("Restart")) {
        _scoreboard.push_back(_points);
        _scoreboard.sort();
        _scoreboard.invert();
        _points = 0;
        Math::randomize();
        _player_pos = Vector2(100, 100);
        _chunks.clear();
    }
    ImGui::Text("Scoreboard");
    ImGui::BeginChild("Scrolling");
    for (int i = 0; i < _scoreboard.size(); i++)
        ImGui::Text("%d. %d", i+1, _scoreboard[i]);
    ImGui::EndChild();
    ImGui::End();
    GUI::render();
}

String TestScene::make_key(int cx, int cy) {
    return String::num(cx) + "," + String::num(cy);
}

void TestScene::ensure_chunk(int cx, int cy) {
    String key = make_key(cx, cy);

    if (!_chunks.has(key)) {
        Chunk *c = memnew(Chunk(cx, cy));
        _chunks[key] = c;

        Thread *t = memnew(Thread);
        t->start(thread_func, c);

        ThreadData td;
        td.c=c;
        td.t=t;

        _threads.push_back(td);

    }
}

void TestScene::_manage_threads() {
    for (int i = _threads.size() - 1; i >= 0; i--) {
        if (_threads[i].c->_is_ready.is_set()) {
            _threads[i].t->wait_to_finish();
            memdelete(_threads[i].t);
            _threads.remove(i);
        }
    }
}

bool TestScene::is_solid_tile(float world_x, float world_y) {
    int tile_x = (int)floor(world_x / _tile_size);
    int tile_y = (int)floor(world_y / _tile_size);

    int cx = (int)floor((float)tile_x / _chunk_size);
    int cy = (int)floor((float)tile_y / _chunk_size);

    String key = make_key(cx, cy);

    if(!_chunks.has(key))
        return true; // ha nem letezik, akkor falkent kezeli

    Chunk *c = _chunks[key];
    if(!c->_is_ready.is_set())
        return true;

    int local_x = tile_x - cx * _chunk_size;
    int local_y = tile_y - cy * _chunk_size;

    int tile = c->get(local_x, local_y);
    return tile == 1;
}

void TestScene::check_diamond_pickup() {
    int tile_x = (int)floor(_player_pos.x / _tile_size);
    int tile_y = (int)floor(_player_pos.y / _tile_size);

    int cx = (int)floor((float)tile_x / _chunk_size);
    int cy = (int)floor((float)tile_y / _chunk_size);

    String key = make_key(cx, cy);

    if(!_chunks.has(key))
        return;

    Chunk *c = _chunks[key];
    if(!c->_is_ready.is_set())
        return;

    int local_x = tile_x - cx * _chunk_size;
    int local_y = tile_y - cy * _chunk_size;

    int tile = c->get(local_x, local_y);

    if(tile == 2) {
        c->set(local_x, local_y, 0);
        _points++;
    }
}

TestScene::TestScene() {
    Math::randomize();

    _tile_size = 75;
    _chunk_size = 8;
    Chunk::CHUNK_SIZE = _chunk_size;
    _view_radius = 2;
    _player_size = (int)(_tile_size * 0.85f);

    ensure_chunk(0, 0);

    _font.instance();
    _font->load_default(31.5);

    _grass_image.instance();
    _grass_image->load_from_file("grass.png");
    _wall_image.instance();
    _wall_image->load_from_file("wall.png");
    _diamond_image.instance();
    _diamond_image->load_from_file("diamond.png");
    _player_image.instance();
    _player_image->load_from_file("player.png");

    _grass_texture.instance();
    _grass_texture->create_from_image(_grass_image);
    _wall_texture.instance();
    _wall_texture->create_from_image(_wall_image);
    _diamond_texture.instance();
    _diamond_texture->create_from_image(_diamond_image);
    _player_texture.instance();
    _player_texture->create_from_image(_player_image);

    _player_pos = Vector2(100, 100);
    _player_speed = 200;
    _points = 0;


    up = false;
    down = false;
    right = false;
    left = false;


}
