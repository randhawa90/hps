import java.awt.geom.Point2D;
import java.io.IOException;

import javax.swing.SwingWorker;


public class RandomPrey extends RandomPlayer implements EvasionListener{

  public RandomPrey(EvasionModel model, int N, int W) {
    super(model, N, W);
    // TODO Auto-generated constructor stub
  }

  @Override
  public void hunter_moved(HunterMove move, String message) throws IOException {
    moveCount++;
    if (moveCount %2 ==0) {
      final PreyMoves preyMove = PreyMoves.values()[randGen.nextInt(PreyMoves.values().length)];
      new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        model.preyMove(preyMove);
        return 0;
      }
    }.execute();
    }
  }

  @Override
  public void prey_moved(Point2D move) throws IOException {
    
    
  }

  @Override
  public void prey_caught(long no_of_moves) throws IOException {
    
    
  }

  @Override
  public void game_started(String hunterName, String preyName)
      throws IOException {
    // TODO Auto-generated method stub
    
  }

  @Override
  public void game_reset() {
    // TODO Auto-generated method stub
    
  }

  @Override
  public void time_over(long no_of_moves, char player) throws IOException {
    // TODO Auto-generated method stub
    
  }

  @Override
  public String getName() {
    return "RandomPrey";
  }

}
