#include "test_scene.h"

void TestScene::input_event(const Ref<InputEvent> &event){
    Ref<InputEventKey> iek = event;

    if(iek.is_valid()) {
        if(!iek->is_pressed()) {
            return;
        }
        if(iek->get_physical_scancode() == KEY_K) {
            if(!_test_keys.has("K")) {
                _test_keys["K"] = 1;
            }
            else {
                _test_keys["K"] = _test_keys["K"] + 1;
            }
        } else if(iek->get_physical_scancode() == KEY_L) {
            if(!_test_keys.has("L")) {
                _test_keys["L"] = 1;
            }
            else {
                _test_keys["L"] = _test_keys["L"] + 1;
            }
        }
    }

}
void TestScene::update(float delta){
}
void TestScene::render(){
    Renderer *r = Renderer::get_singleton();

    r->clear_screen(Color(1, 1, 1));
    r->camera_2d_projection_set_to_window();

    for(int i = 0; i < _chunk_size; i++){
        for(int j = 0; j < _chunk_size; j++){

            if(i % 2 == 0 && j % 2 == 0){
                r->draw_texture(_grass_texture, Rect2(0 + i * _tile_size, 0 + j * _tile_size, _tile_size, _tile_size));
            }
            else
            {
                r->draw_texture(_wall_texture, Rect2(0 + i * _tile_size, 0 + j * _tile_size, _tile_size, _tile_size));
            }
        }
    }
    //r->draw_texture(_grass_texture, Rect2(400, 400, 100, 100));
    //r->draw_texture(_wall_texture, Rect2(600, 600, 100, 100));
    //r->draw_texture_clipped(_texture, Rect2(32, 32, 16, 16), Rect2(400, 600, 100, 100));
    r->draw_text_2d("Points: ", _font, Vector2(5,5), Color(1,0,0));
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

    _tile_size = 50;
    _chunk_size = 10;
}
