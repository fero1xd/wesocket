const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{ .name = "main", .root_module = b.createModule(.{ .target = target, .optimize = optimize }) });

    exe.root_module.addCSourceFiles(.{ .files = &.{
        "main.c",
    }, .flags = &.{ "-Wall", "-Wextra", "-std=c23", "-xc" } });

    exe.root_module.addIncludePath(.{ .cwd_relative = "/opt/homebrew/Cellar/openssl@3/3.6.1/include" });
    exe.root_module.addLibraryPath(.{ .cwd_relative = "/opt/homebrew/Cellar/openssl@3/3.6.1/lib" });

    exe.root_module.linkSystemLibrary("ssl", .{});
    exe.root_module.linkSystemLibrary("crypto", .{});
    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
