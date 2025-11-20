#include "test_scene.h"

void TestScene::input_event(const Ref<InputEvent> &event){
    Ref<InputEventKey> iek = event;

      if(iek.is_valid()) {
        if(!iek->is_pressed())
            return;

        if(iek->get_physical_scancode() == KEY_W)
            _player_pos.y -= _player_speed;
        if(iek->get_physical_scancode() == KEY_S)
            _player_pos.y += _player_speed;
        if(iek->get_physical_scancode() == KEY_A)
            _player_pos.x -= _player_speed;
        if(iek->get_physical_scancode() == KEY_D)
            _player_pos.x += _player_speed;
    }

}
void TestScene::update(float delta){
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

    int player_chunk_x = floor(_player_pos.x / (_tile_size * _chunk_size));
    int player_chunk_y = floor(_player_pos.y / (_tile_size * _chunk_size));

    for (int y=-_view_radius; y<=_view_radius; y++) {
        for (int x=-_view_radius; x<=_view_radius; x++) {

            int cx = player_chunk_x + x;
            int cy = player_chunk_y + y;

            String key = make_key(cx, cy);

            if (!_chunks.has(key))
                continue;

            Chunk& c = _chunks[key];

            int world_x = cx * _chunk_size * _tile_size;
            int world_y = cy * _chunk_size * _tile_size;

            for (int y=0; y<_chunk_size; y++) {
                for (int x=0; x<_chunk_size; x++) {

                    int tile = c.get(x,y);

                    r->draw_texture((tile==0) ? _grass_texture : _wall_texture, Rect2(world_x + x*_tile_size, world_y + y*_tile_size, _tile_size, _tile_size));
                }
            }
        }
    }


    r->draw_rect(Rect2(_player_pos.x - 1, _player_pos.y - 1, 50, 50), Color(0, 0, 0));

    //r->draw_texture(_grass_texture, Rect2(400, 400, 100, 100));
    //r->draw_texture(_wall_texture, Rect2(600, 600, 100, 100));
    //r->draw_texture_clipped(_texture, Rect2(32, 32, 16, 16), Rect2(400, 600, 100, 100));
    r->draw_text_2d("Points: ", _font, Vector2(5,5), Color(1,0,0));
}

String TestScene::make_key(int cx, int cy) {
    return String::num(cx) + "," + String::num(cy);
}

void TestScene::ensure_chunk(int cx, int cy) {
    String key = make_key(cx, cy);

    if (!_chunks.has(key)) {
        Chunk c(cx, cy, _chunk_size);
        _chunks[key] = c;
    }
}


TestScene::TestScene() {
    _font.instance();
    _font->load_default(31.5);

    _grass_image.instance();
    _grass_image->load_from_file("grass.png");
    _wall_image.instance();
    _wall_image->load_from_file("wall.png");

    _grass_texture.instance();
    _grass_texture->create_from_image(_grass_image);
    _wall_texture.instance();
    _wall_texture->create_from_image(_wall_image);

    _player_pos = Vector2(100, 100);
    _player_speed = 50;

    _tile_size = 50;
    _chunk_size = 5;
    _view_radius = 1;

    ensure_chunk(0, 0);
}
