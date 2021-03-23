#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import gam
import contrib.gam_iec as gam_iec
from scipy.spatial.transform import Rotation

# global log level
gam.log.setLevel(gam.DEBUG)

# create the simulation
sim = gam.Simulation()

# verbose and GUI
sim.set_g4_verbose(False)
sim.set_g4_visualisation_flag(False)

# set random engine
sim.set_g4_random_engine("MersenneTwister", 987654)

#  change world size
m = gam.g4_units('m')
cm = gam.g4_units('cm')
world = sim.world
world.size = [1 * m, 1 * m, 1 * m]

# add a iec phantom
iec_phantom = gam_iec.add_phantom(sim)
iec_phantom.translation = [3 * cm, 1 * cm, 0 * cm]
iec_phantom.rotation = Rotation.from_euler('y', 33, degrees=True).as_matrix()

# simple source
# gam_iec.add_sources(sim, 'iec', 'all')
kBq = gam.g4_units('Bq') * 1000
ac = 2 * kBq
gam_iec.add_sources(sim, 'iec',
                    [10, 13, 17, 22, 28, 37],
                    [ac, ac, ac, ac, ac, ac])

# add stat actor
stats = sim.add_actor('SimulationStatisticsActor', 'stats')
stats.track_types_flag = True

# add dose actor
dose = sim.add_actor('DoseActor', 'dose')
dose.save = 'output/test015.mhd'
dose.attached_to = 'iec'
dose.dimension = [100, 100, 100]
mm = gam.g4_units('mm')
dose.spacing = [2 * mm, 2 * mm, 2 * mm]

# run timing
sec = gam.g4_units('second')
sim.run_timing_intervals = [[0, 1 * sec]]

print(sim.volume_manager.dump_tree())
print(sim.source_manager.dump(0))

# initialize & start
sim.initialize()
sim.start()

# Only for reference stats:
stats = sim.get_actor('stats')
#stats.write('output/test015_stats.txt')

# check
stats_ref = gam.read_stat_file('./output/test015_stats.txt')
is_ok = gam.assert_stats(stats, stats_ref, 0.05)
is_ok = gam.assert_images('output/test015.mhd', 'output/test015_ref.mhd', tolerance=0.2)

gam.test_ok(is_ok)