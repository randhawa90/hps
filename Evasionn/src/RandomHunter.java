import java.awt.Point;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.SwingWorker;


public class RandomHunter extends RandomPlayer implements EvasionListener {
  int movesToWall;
  HunterMoves direction;
  Point2D loc;
  ArrayList<Line2D> walls;
  public RandomHunter(EvasionModel model, int N, int W) {
    super(model, N, W);
    movesToWall = N;
    walls = new ArrayList<Line2D>();
    direction = HunterMoves.SE;
    loc = new Point(0,0);
  }

  @Override
  public void hunter_moved(HunterMove move, String message) throws IOException {
    moveCount++;
    direction = move.move;
    loc = move.position;
    if (movesToWall > 0) {
      movesToWall--;
    }
    if (move.madeWall) {
      movesToWall = N;
      walls.add(new Line2D.Float(move.start, move.end));
    }
    if (move.deletedWall) {
      walls.remove(move.wallNumber-1);
    }
    if (moveCount %2 == 1) {
      new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        model.hunterMove(makeMove());
        return 0;
      }
    }.execute();
    }
  }

  @Override
  public void prey_moved(Point2D move) throws IOException {
    new SwingWorker<Integer, Integer>() {
    @Override
    protected Integer doInBackground() throws Exception {
      model.hunterMove(makeMove());
      return 0;
    }
  }.execute();

  }

  private HunterMove makeMove() {
    HunterMove move = new HunterMove();
    boolean makeOrBreak = randGen.nextBoolean();
    if (movesToWall == 0 && makeOrBreak && walls.size() < (W-(W)/8)) {
      switch (direction) {
        case NE:
          move.move = HunterMoves.NEw;
          break;
        case NW:
          move.move = HunterMoves.NWw;
          break;
        case SE:
          move.move = HunterMoves.SEw;
          break;
        case SW:
          move.move = HunterMoves.SWw;
          break;
        default:
          break;
      }
      boolean hOrV = randGen.nextBoolean();
      boolean zOrF = randGen.nextBoolean();
      Point2D end;
      if (hOrV) {
        if (zOrF) {
          end = new Point2D.Float(0,(int)loc.getY());
        }
        else {
          end = new Point2D.Float(499,(int)loc.getY());
        }
      }
      else {
        if (zOrF) {
          end = new Point2D.Float((int)loc.getX(),0);
        }
        else {
          end = new Point2D.Float((int)loc.getX(),499);
        }
      }
      move.start = loc;
      move.end = end;
      return move;
    }
    if (walls.size() > (W-(1*W)/4)&&randGen.nextBoolean()) {
      if (randGen.nextBoolean()) {
        int removal = randGen.nextInt(walls.size()) + 1;
        switch (direction) {
          case NE:
            move.move = HunterMoves.NEwx;
            break;
          case NW:
            move.move = HunterMoves.NWwx;
            break;
          case SE:
            move.move = HunterMoves.SEwx;
            break;
          case SW:
            move.move = HunterMoves.SWwx;
            break;
          default:
            break;

        }
        move.wallNumber = removal;
        return move;
      }
    }
    move.move = direction;
    return move;
  }
  @Override
  public void prey_caught(long no_of_moves) throws IOException {
    // TODO Auto-generated method stub

  }

  @Override
  public void game_started(String hunterName, String preyName)
      throws IOException {
    model.hunterMove(makeMove());

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
    return "RandomHunter";
  }

}
