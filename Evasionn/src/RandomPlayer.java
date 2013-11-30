import java.util.Random;


public class RandomPlayer {
  long moveCount;
  Random randGen;
  EvasionModel model;
  int N;
  int W;
  public RandomPlayer(EvasionModel model, int N, int W) {
    randGen = new Random(new Random().nextInt());
    this.model = model;
    moveCount = 0;
    this.N = N;
    this.W = W;
  }
}
