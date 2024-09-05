degree_ir = 30;
degree_vis = 20;
Rs_ir = csvread("TSL2591_Rs_IR.csv");
Rs_vis = csvread("TSL2591_Rs_VIS.csv");

p_ir = polyfit(Rs_ir(:,1), Rs_ir(:,2), degree_ir);
p_vis = polyfit(Rs_vis(:,1), Rs_vis(:,2), degree_vis);

y_ir = polyval(p_ir, Rs_ir(:,1));
y_vis = polyval(p_vis, Rs_vis(:,1));


figure();
title("TSL2591_Rs_IR fir");
plot(Rs_ir(:,1), Rs_ir(:,2));
hold on;
plot(Rs_ir(:,1), y_ir);
hold off;
legend("original", "fit");

figure();
title("TSL2591_Rs_VIS fir");
plot(Rs_vis(:,1), Rs_vis(:,2));
hold on;
plot(Rs_vis(:,1), y_vis);
hold off;
legend("original", "fit");