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
    struct ThreadData {
        Chunk* c;
        Thread* t;
    };
    Vector<ThreadData> _threads;

    Ref<Font> _font;

    Ref<Image> _grass_image;
    Ref<Image> _wall_image;
    Ref<Image> _diamond_image;
    Ref<Image> _player_image;
    Ref<Texture> _grass_texture;
    Ref<Texture> _wall_texture;
    Ref<Texture> _diamond_texture;
    Ref<Texture> _player_texture;

    Vector2 _player_pos;
    float _player_speed;

    bool up;
    bool down;
    bool right;
    bool left;

    int _player_size;
    int _tile_size;
    int _chunk_size;
    int _view_radius;

    int _points;

    HashMap<String, Chunk*> _chunks;

    void ensure_chunk(int cx, int cy);
    void _manage_threads();
    String make_key(int cx, int cy);
    bool is_solid_tile(float world_x, float world_y);
    void check_diamond_pickup();

};

#endif // TEST_SCENE_H
