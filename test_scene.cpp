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

    if(up){
        _player_pos.y -= _player_speed * delta;
    }
    else if(down){
        _player_pos.y += _player_speed * delta;
    }
    else if(right){
        _player_pos.x += _player_speed * delta;
    }
    else if(left){
        _player_pos.x -= _player_speed * delta;
    }

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

                    r->draw_texture((tile==0) ? _grass_texture : _wall_texture, Rect2(cam.x + world_x + x * _tile_size, cam.y + world_y + y * _tile_size, _tile_size, _tile_size));
                }
            }
        }
    }

    r->draw_texture(_player_texture,Rect2(cam.x + _player_pos.x - 25,cam.y + _player_pos.y - 25,50,50));

    r->draw_text_2d("Points: ", _font, Vector2(5,5), Color(1,0,0));
}

String TestScene::make_key(int cx, int cy) {
    return String::num(cx) + "," + String::num(cy);
}

void TestScene::ensure_chunk(int cx, int cy) {
    String key = make_key(cx, cy);

    if (!_chunks.has(key)) {
        Chunk *c = memnew(Chunk(cx, cy, _chunk_size));
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


TestScene::TestScene() {
    _font.instance();
    _font->load_default(31.5);

    _grass_image.instance();
    _grass_image->load_from_file("grass.png");
    _wall_image.instance();
    _wall_image->load_from_file("wall.png");
    _player_image.instance();
    _player_image->load_from_file("player.png");

    _grass_texture.instance();
    _grass_texture->create_from_image(_grass_image);
    _wall_texture.instance();
    _wall_texture->create_from_image(_wall_image);
    _player_texture.instance();
    _player_texture->create_from_image(_player_image);

    _player_pos = Vector2(100, 100);
    _player_speed = 200;


    up = false;
    down = false;
    right = false;
    left = false;

    _tile_size = 50;
    _chunk_size = 5;
    _view_radius = 1;

    ensure_chunk(0, 0);
}
