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
import java.applet.*;
import java.awt.*;
import javax.swing.*;

public class TestApplet extends JApplet {
  Object lock = new Object();
  class GPanel extends JPanel {
    /**
     * 
     */
    private static final long serialVersionUID = 4626419896777520312L;
    Graphics2D g2d;
    int height, width;

    public GPanel(int height, int width) {
      this.height = height;
      this.width = width;
      setPreferredSize(new Dimension(this.height, this.width));
    }

    public GPanel() {
      this(400, 400);
    }

    @Override
    public void paintComponent(Graphics g) {
      super.paintComponent(g);
      g2d = (Graphics2D)g;
      g2d.setBackground(Color.WHITE);
      g2d.clearRect(0, 0, this.height, this.width);
      g2d.setColor(Color.GREEN);
      g2d.setStroke(new BasicStroke(1f));
      synchronized (lock) {
        for (Line2D wall : walls) {
          g2d.draw(wall);
        }
      }
      g2d.setColor(Color.BLACK);
      g2d.draw(new Line2D.Float(0,this.width,this.height,this.width));
      g2d.draw(new Line2D.Float(0,0, this.height, 0));
      g2d.draw(new Line2D.Float(0,0,0, this.width));
      g2d.draw(new Line2D.Float(this.width, 0, this.height, this.width));
      if (preyPosition != null) {
        g2d.setColor(Color.BLUE);
        g2d.setStroke(new BasicStroke(3f,BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        g2d.draw(new Line2D.Float(preyPosition,preyPosition));
      }
      if (hunterPosition != null) {
        g2d.setColor(Color.RED);
        g2d.setStroke(new BasicStroke(5f));
        g2d.draw(new Line2D.Float(hunterPosition,hunterPosition));
      }
    }
  }
  final EvasionModel model = null;
  String name;
  long move_counter;
  long time_counter;

  int moves_to_next_wall;
  Point2D preyPosition;
  Point2D hunterPosition;
  HunterMoves hunterDirection;
  ArrayList<Line2D> walls = new ArrayList<Line2D>();
  JFrame frame;
  JTextArea gameDescription;
  JButton hunterButton;

  /* attributes needed by the game */
  final static int HUMAN = 0;
  final static int COMPUTER = 1;
  final int N = 3;
  final int W = 3;
  int hunter = COMPUTER;
  int prey = COMPUTER;

  String hunterName;
  String preyName;
  
  GPanel mainPanel = new GPanel();
  /* GUI components */
  JLabel displayString = new JLabel("DisplayLabel");
  JLabel hunterLabel = new JLabel("HunterLabel");
  JLabel preyLabel = new JLabel("PreyLabel");


  String[] nstrings = {"3", "4", "5", "6", "7", "8", "9", "10"};
  JComboBox NList = new JComboBox(nstrings);
  //NList.setSelectedIndex(0);
  NList.addActionListener(new ActionListener {
    public void actionPerformed(ActionEvent e) {
          JComboBox cb = (JComboBox)e.getSource();
          String str = (String)cb.getSelectedItem();
          N = Integer.parseInt(str);
      }
  }
  );

  /*
  String wstrings = {"3", "4", "5", "6", "7", "8", "9", "10"};
  JComboBox WList = new JComboBox(wstrings);
  WList.setSelectedIndex(1);
  WList.addActionListener(new ActionListener {
    public void actionPerformed(ActionEvent e) {
          JComboBox cb = (JComboBox)e.getSource();
          String str = (String)cb.getSelectedItem();
          W = Integer.parseInt(str);
      }
  }
  );
 
  class RadioActionListener implements ActionListener {
    int id = 0;
    publid RadioActionListener(int id) {
      this.id = id;
    }
    public void actionPerformed(ActoinEvent e) {
      if(this.id == 0) {// hunter
        if ("Computer".equals(e.getActionCommand())) {
          hunter = COMPUTER;
        }else {
          hunter = HUMAN;
        }
      }
      else {
        if ("Computer".equals(e.getActionCommand())) {
          prey = COMPUTER;
        }else {
          prey = HUMAN;
        }
      }
    }
  }
  ButtonGroup hunterBG = new ButtonGroup();
  JRadioButton hunterComputer = new JRatioButton("Computer");
  JRadioButton hunterHuman = new JRatioButton("Human");
  hunterBG.add(hunterComputer);
  hunterBG.add(hunterHuman);
  hunterComputer.setSelected(true);
  hunterComputer.addActionListener(new RadioActionListener(0));
  hunterHuman.addActionListener(new RadioActionListener(0));
  
  ButtonGroup preyBG = new ButtonGroup();
  JRadioButton preyComputer = new JRatioButton("Computer");
  JRadioButton preyHuman = new JRatioButton("Human");
  preyBG.add(preyComputer);
  preyBG.add(preyHuman);
  preyComputer.setSelected(true);
  preyComputer.addActionListener(new RadioActionListener(1));
  preyHuman.addActionListener(new RadioActionListener(1));
  */

  JButton startButton = new JButton("Start");
  JButton resetButton = new JButton("Reset");

  /*
  resetButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
          walls = ArrayList<Line2D>();
          hunter = COMPUTER;
          prey = COMPUTER;
          N = 3;
          W = 3;
      }
    }
  });

  */
  public void init() {
    Container container = getContentPane();
    container.setBackground(Color.WHITE);
    container.setLayout(new BorderLayout());
    container.add(displayString, BorderLayout.PAGE_START);
    
    JPanel gamePanel = new JPanel();
    container.add(gamePanel, BorderLayout.CENTER);
    
    gamePanel.setLayout(new FlowLayout());
    gamePanel.add(mainPanel);
    JPanel controlPanel = new JPanel();
    gamePanel.add(controlPanel);
    controlPanel.setLayout(new BoxLayout(controlPanel, BoxLayout.Y_AXIS));
    controlPanel.add(NList);
    //controlPanel.add(WList);
    controlPanel.add(hunterLabel);
    //controlPanel.add(hunterBG);
    controlPanel.add(preyLabel);
    //controlPanel.add(preyBG);
    controlPanel.add(startButton);
    controlPanel.add(resetButton);
  }
}
