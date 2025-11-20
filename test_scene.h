#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "sfw.h"
#include "chunk.h"

class TestScene : public Scene {
    SFW_OBJECT(TestScene, Scene);

public:
    virtual void input_event(const Ref<InputEvent> &event);
	virtual void update(float delta);
	virtual void render();

	TestScene();

protected:
    Ref<Font> _font;

    Ref<Image> _grass_image;
    Ref<Image> _wall_image;
    Ref<Texture> _grass_texture;
    Ref<Texture> _wall_texture;

    Vector2 _player_pos;
    float _player_speed;

    int _tile_size;
    int _chunk_size;
    int _view_radius;

    HashMap<String, Chunk> _chunks;

    void ensure_chunk(int cx, int cy);
    String make_key(int cx, int cy);

};

#endif // TEST_SCENE_H
