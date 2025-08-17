module;

export module game.scene;
export import game.context;

export class IScene {
public:
  virtual ~IScene() = default;

  /**
   * @brief シーンに入る時一回だけ呼ぶ
   */
  virtual void OnEnter(GameContext* ctx) = 0;

  /**
   * @brief フレーム毎に呼ふ
   */
  virtual void OnUpdate(GameContext* ctx, float delta_time) = 0;

  /**
   * @brief フレーム毎に呼ふ
   */
  virtual void OnRender(GameContext* ctx) = 0;

  /**
   * @brief 固定の頻度に呼ふ
   */
  virtual void OnFixedUpdate(GameContext* ctx, float delta_time) = 0;

  /**
   * @brief シーンから出る時一回た?け呼ふ?
   */
  virtual void OnExit(GameContext* ctx) = 0;
};
