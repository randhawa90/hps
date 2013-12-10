import java.awt.Point;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.SwingWorker;


public class Prey  implements EvasionListener {
  final EvasionModel model;
  String name;
  int move_counter;
  long time_counter;
  final int N;
  final int W;
  int moves_to_next_wall;
  Point2D preyPosition;
  Point2D hunterPosition;
  HunterMoves hunterDirection;
  ArrayList<Line2D> walls;
  public Prey(EvasionModel model, int N, int W) {
//    super(model, N, W, totalTime, portNumber);
    this.model = model;
    this.N = N;
    this.W = W;
    preyPosition = new Point(130,100);
    hunterPosition = new Point(0, 0);
    moves_to_next_wall = N;
    move_counter = 0;
    hunterDirection = HunterMoves.SE;
    walls = new ArrayList<Line2D>();
  }
  
  public void make_move(PreyMoves move,long wait) throws IOException {
    long time = System.currentTimeMillis();
    while(System.currentTimeMillis() - time < wait);
    model.preyMove(move);
  }

  public void make_move() throws IOException {
    PreyMoves move = PreyMoves.ZZ;
    model.preyMove(move);
  }

  @Override
  public void hunter_moved(final HunterMove move, String message) throws IOException {
    // TODO Auto-generated method stub
    move_counter++;
    if (moves_to_next_wall > 0) {
      moves_to_next_wall--;
    }
    hunterPosition = move.position;
    if (move.madeWall) {
      moves_to_next_wall = N;
      walls.add(new Line2D.Float(move.start, move.end));
    }
    else if (move.deletedWall) {
      walls.remove(move.wallNumber-1);
    }
    hunterDirection = move.move;
  }

  @Override
  public void prey_moved(Point2D move) {
    preyPosition = move;
  }

  @Override
  public void prey_caught(long no_of_moves) {
  }

  @Override
  public void game_started(String hunterName, String preyName) {
    
  }

  @Override
  public void game_reset() {
    
  }

  @Override
  public void time_over(long no_of_moves, final char player) {
  }

  @Override
  public String getName() {
    return name;
  }

}
