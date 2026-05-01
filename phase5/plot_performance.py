import csv

import matplotlib.pyplot as plt


def load_data(csv_file):
    order_sizes = []
    execution_times = []
    throughputs = []

    with open(csv_file, newline="", encoding="utf-8") as file:
        reader = csv.DictReader(file)
        for row in reader:
            order_sizes.append(int(row["order_volume"]))
            execution_times.append(float(row["execution_time_seconds"]))
            throughputs.append(float(row["throughput_ops_per_second"]))

    return order_sizes, execution_times, throughputs


def plot(order_sizes, execution_times, throughputs, output_file):
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    axes[0].plot(order_sizes, execution_times, marker="o", linestyle="-")
    axes[0].set_xlabel("Number of Orders")
    axes[0].set_ylabel("Execution Time (seconds)")
    axes[0].set_title("HFT Order Book: Execution Time")
    axes[0].grid(True)

    axes[1].plot(order_sizes, throughputs, marker="o", linestyle="-", color="green")
    axes[1].set_xlabel("Number of Orders")
    axes[1].set_ylabel("Throughput (ops/s)")
    axes[1].set_title("HFT Order Book: Throughput")
    axes[1].grid(True)

    fig.tight_layout()
    fig.savefig(output_file, dpi=200)


if __name__ == "__main__":
    sizes, times, ops = load_data("performance_data_unoptimized.csv")
    plot(sizes, times, ops, "performance_chart_unoptimized.png")

    sizes, times, ops = load_data("performance_data_optimized.csv")
    plot(sizes, times, ops, "performance_chart_optimized.png")
