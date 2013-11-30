import java.awt.BasicStroke;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.ArrayList;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.SwingWorker;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;


public class EvasionView implements EvasionListener {
  Object lock = new Object();
  class GPanel extends JPanel {
    /**
     * 
     */
    private static final long serialVersionUID = 4626419896777520312L;
    Graphics2D g2d;
    public GPanel() {
      setPreferredSize(new Dimension(500, 500));
    }

    @Override
    public void paintComponent(Graphics g) {
      super.paintComponent(g);
      g2d = (Graphics2D)g;
      g2d.setBackground(Color.WHITE);
      g2d.clearRect(0, 0, 500, 500);
      g2d.setColor(Color.GREEN);
      g2d.setStroke(new BasicStroke(1f));
      synchronized (lock) {
        for (Line2D wall : walls) {
          g2d.draw(wall);
        }
      }
      g2d.setColor(Color.BLACK);
      g2d.draw(new Line2D.Float(0,500,500,500));
      g2d.draw(new Line2D.Float(0,0, 500, 0));
      g2d.draw(new Line2D.Float(0,0,0, 500));
      g2d.draw(new Line2D.Float(500, 0, 500, 500));
      g2d.setColor(Color.BLUE);
      g2d.setStroke(new BasicStroke(3f,BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
      g2d.draw(new Line2D.Float(preyPosition,preyPosition));
      g2d.setColor(Color.RED);
      g2d.setStroke(new BasicStroke(5f));
      g2d.draw(new Line2D.Float(hunterPosition,hunterPosition));
    }
  }
  final EvasionModel model;
  String name;
  long move_counter;
  long time_counter;
  final int N;
  final int W;
  int moves_to_next_wall;
  Point2D preyPosition;
  Point2D hunterPosition;
  HunterMoves hunterDirection;
  ArrayList<Line2D> walls;
  String displayString;
  JFrame frame;
  GPanel mainPanel;
  JTextArea gameDescription;
  JButton hunterButton;
  Graphics2D graphics;

  public EvasionView(EvasionModel model, int N, int W) throws ClassNotFoundException, InstantiationException, IllegalAccessException, UnsupportedLookAndFeelException {
    this.model = model;
    this.N = N;
    this.W = W;
    preyPosition = new Point(330,200);
    hunterPosition = new Point(0, 0);
    moves_to_next_wall = N;
    time_counter = 0;
    move_counter = 0;
    hunterDirection = HunterMoves.SE;
    walls = new ArrayList<Line2D>();

    UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
    frame = new JFrame("Evasion");
    gameDescription = new JTextArea();
    mainPanel = new GPanel();
    mainPanel.setSize(500, 500);
    mainPanel.setBackground(Color.WHITE);
    //    mainPanel.setBorder(BorderFactory.createLineBorder(Color.black, 1));
    mainPanel.setVisible(true);
    Container frameContainer = frame.getContentPane();
    frameContainer.add(mainPanel,BorderLayout.CENTER);
    frameContainer.add(gameDescription,BorderLayout.NORTH);
    frame.setSize(510, 540);
    frame.setMaximumSize(new Dimension(520, 520));
    frame.setLocation(200, 200);
    //    frame.setExtendedState(JFrame.);
    frame.setVisible(true);
    frame.setResizable(false);
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.setVisible(true);
  }

  @Override
  public void hunter_moved(HunterMove move, final String message) {
    move_counter++;
    hunterPosition = move.position;
    if (move.madeWall) {
      synchronized (lock) {
        walls.add(new Line2D.Float(move.start, move.end));
      }

    }
    else if (move.deletedWall) {
      synchronized (lock) {
        walls.remove(move.wallNumber-1);
      }
    }
    hunterDirection = move.move;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        frame.getContentPane().validate();frame.getContentPane().repaint();
        gameDescription.setText(displayString + move_counter + message);
        return 0;
      }
    }.execute();
  }

  @Override
  public void prey_moved(Point2D move) {
    preyPosition = move;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        mainPanel.repaint();
        return 0;
      }
    }.execute();
  }

  @Override
  public void prey_caught(long no_of_moves) {
    gameDescription.setText(displayString + ++move_counter + "Hunter Won!!!");
  }

  @Override
  public void game_started(final String hunterName,final String preyName) {
    displayString = "Hunter " + hunterName + " vs Prey " + preyName +". Moves:";
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        mainPanel.repaint();
        frame.repaint();
        gameDescription.setText(displayString + move_counter);
        return 0;
      }
    }.execute();
  }

  @Override
  public void game_reset() {
    

  }

  @Override
  public void time_over(long no_of_moves, final char player) {
    move_counter = no_of_moves;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        switch (player) {
          case 'h':
            gameDescription.setText(displayString + move_counter +"Hunter Timed Out!!!");
            break;

          case 'p':
            gameDescription.setText(displayString + move_counter+"Prey Timed Out!!!");
            break;

          default:
            break;
        }
        frame.repaint();
        return 0;
      }
    }.execute();
  }

  @Override
  public String getName() {
    return null;
  }

}
