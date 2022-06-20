import json
import copy
import graphviz
from IPython.display import display

def isnotebook():
    # https://stackoverflow.com/questions/15411967/how-can-i-check-if-code-is-executed-in-the-ipython-notebook
    try:
        shell = get_ipython().__class__.__name__
        if shell == 'ZMQInteractiveShell':
            return True   # Jupyter notebook or qtconsole
        elif shell == 'TerminalInteractiveShell':
            return False  # Terminal running IPython
        else:
            return False  # Other type (?)
    except NameError:
        return False      # Probably standard Python interpreter

if isnotebook():
    from tqdm.notebook import tqdm
else:
    from tqdm import tqdm

class StatCalculator:
    def __init__(self):
        self.sum = 0
        self.count = 0
        self.is_next = True
    
    def add(self, value=1, requires_next=False):
        if requires_next:
            if self.is_next:
                self.is_next = False
                self.sum += value
        else:
            self.sum += value
        
    def next_compute(self):
        self.count += 1
        self.is_next = True
        
    def get_stat(self):
        p = self.sum / self.count
        return p
    
    def __repr__(self):
        p = self.get_stat()
        return f"(prop={p})"

def get_track_by_id(event, i):
    for t in event:
        if t["TrackID"] == i:
            return t
    return None

def get_chain_name(chain):
    models = []
    
    for link in chain:
        if "nishina" in link["ClassName"].lower():
            return link["ClassName"]
    
    for link in chain:
        if "livermorephotoelectricmodel" in link["ClassName"].lower():
            return link["ClassName"]
    
    for link in chain:
        if "model" in link["ClassName"].lower():
            return link["ClassName"]
        
    if "" in models:
        return ""
    
    for link in chain[::-1]:
        if "process" in link["ClassName"].lower():
            return link["ClassName"]
        
    return "Unkown"

def get_call_name(call):
    return call["ClassName"] + "::" + call["FuncName"]
    
def get_chain(event, track):
    total = []
    node = track
    end_chain = len(node["Calls"])
    
    while node["ParentID"] != -1:
        current_end_chain = end_chain
        
        local_chain = node["Calls"][:end_chain]
        local_steps = node["Steps"][:end_chain]
        
        node_step = node["Steps"][0]["InteractionNumber"]
        parent = get_track_by_id(event, node["ParentID"])
        
        if parent is not None:
            for i, step in enumerate(parent["Steps"]):
                    if step["InteractionNumber"] == node_step:
                        end_chain = i
                        break
        
            total.append((local_chain, local_steps, node["TrackID"], node["Particle"], node_step))
            node = parent
        else:
            total.append(([], [], 0, "gamma", []))
            break
            
    return list(reversed(total))

def break_chain_by_interaction(chain, steps):
    chain_list = []
    steps_list = []
    current_interaction = -1
    
    for step, elem in zip(steps, chain):
        if step["InteractionNumber"] != current_interaction:
            chain_list.append([])
            steps_list.append([])
            current_interaction = step["InteractionNumber"]
            
        chain_list[-1].append(elem)
        steps_list[-1].append(step)
        
    return chain_list, steps_list
    
def compute_probas(events, energy = False, compressed = False, count = True):
    by_interaction = {}
    
    for i, event in tqdm(enumerate(events), desc="Event", total=len(events)):
        # print(i)
        # for track in tqdm(event, desc="Track", total=len(event), leave=False):
        for track in event:
            chain = get_chain(event, track)
            
            computed_begin_id = "Primary"
            for total_chain, total_steps, part_id, part_type, step in chain:
                if part_type != "gamma":
                    pass
                
                if compressed:
                    local_chain_loop, local_steps_loop = [total_chain], [total_steps]
                else:
                    local_chain_loop, local_steps_loop = break_chain_by_interaction(total_chain, total_steps)

                computed_begin_id

                for local_steps, local_chain in zip(local_steps_loop, local_chain_loop):
                    chain_id = f"{get_chain_name(local_chain)}"
                    computed_begin_id += f"-{chain_id}"

                    already_seen = ()

                    update_stats = False
                    if computed_begin_id not in by_interaction:
                        by_interaction[computed_begin_id] = {}
                        by_interaction[computed_begin_id]["type"] = part_type
                        by_interaction[computed_begin_id]["part"] = []
                        by_interaction[computed_begin_id]["count"] = 0


                    if (i, part_id) not in by_interaction[computed_begin_id]["part"]:
                        by_interaction[computed_begin_id]["part"].append((i, part_id))
                        by_interaction[computed_begin_id]["count"] += 1

                        for call in local_chain:
                            call_name = get_call_name(call)
                            if call_name not in by_interaction[computed_begin_id]:
                                by_interaction[computed_begin_id][call_name] = StatCalculator()
                            
                            if count:
                                if call["Count"] != 1:
                                    print(call["Count"])
                                by_interaction[computed_begin_id][call_name].add(call["Count"])
                            else:
                                by_interaction[computed_begin_id][call_name].add()
                        
                        if energy:
                            if "MeanEnergy" not in by_interaction[computed_begin_id]: 
                                by_interaction[computed_begin_id]["MeanEnergy"] = StatCalculator()    

                            by_interaction[computed_begin_id]["MeanEnergy"].add(local_steps[0]["TotalEnergy"], True)

                        for cname in by_interaction[computed_begin_id].keys():
                            if cname not in ["type", "part", "count"]:
                                by_interaction[computed_begin_id][cname].next_compute()

    # Compute probability of chains:
    computed = set()
    for k in by_interaction.keys():
        types = k.split('-')
        prefix_length = len(types) - 1
        
        prefix = '-'.join(types[:-1])
        
        if prefix not in computed:
            computed.add(prefix)
            
            count = 0
            to_update = []
            
            # Count how many have the same prefix and length
            for k2 in by_interaction.keys():
                if k2.startswith(prefix): # Same prefix 
                    k2_prefix_length = k2.count('-')
                    if k2_prefix_length == prefix_length: # Same length
                        count += by_interaction[k2]["count"]
                        to_update.append(k2)
                                                
            for tu in to_update:
                by_interaction[tu]["prob"] = by_interaction[tu]["count"] / count
                        
    # Compute total probability of chains
    
    # Fake node to compute total probability
    by_interaction["Primary"] = {"total_prob": 1.0}
    
    for k in by_interaction.keys():
        if k == "Primary":
            continue
            
        types = k.split('-')
        prefix = '-'.join(types[:-1])
        
        by_interaction[k]["total_prob"] = by_interaction[prefix]["total_prob"] * by_interaction[k]["prob"]
        
    return by_interaction

def create_graph(input, add_calls, max_depth, compressed = False, count = False, energy = False):
    """
        input: str
            Path to input json
        add_calls: bool
            True to add calls, False to only add main names
        max_depth:
            Maximum number of depth to show on final graph (note that stats are still computed on the whole
            calling graph)
        compressed: bool
            True if bounces should be compressed (show only part of graph)
        count: bool
            True to show total number of random number asked, False to only show number of calls to function
            Requires add_calls to be True
        energy: bool
            True to show energy in report, False otherwise
            Requires add_calls to be True
    """
    data = json.load(input)
    events = data["PrimaryTracks"]
    probas = compute_probas(events, energy, compressed, count)

    graph = graphviz.Digraph("Probas")

    i = 0
    prefix_to_graph = {}

    graph.node(f"a_{i}", label='Primary')
    prefix_to_graph["Primary"] = f"a_{i}"

    particle_color = {
        "gamma": "green", 
        "e+": "blue", 
        "e-": "red"
    }

    def clear_name(name):
        clear_name = name
        if clear_name.startswith("G4"):
            clear_name = clear_name[2:]
        
        if clear_name.startswith('V'):
            clear_name = clear_name[1:]
            
        return clear_name


    for interaction_name, interaction_data in probas.items():
        if interaction_name == "Primary":
            continue
            
        types = interaction_name.split('-')
        
        if len(types) < max_depth:
            i += 1
            node_name = f"a_{i}"
            prefix_to_graph[interaction_name] = node_name

            prefix = '-'.join(types[:-1])
            suffix = types[-1]

            color = 'black'
            if interaction_data["type"] in particle_color:
                color = particle_color[interaction_data["type"]]
            
            node_label = f"{clear_name(suffix)}"
            if add_calls:
                node_label = '<<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">'
                node_label += f"<TR><TD colspan='2'><b>{clear_name(suffix)}</b></TD></TR>"
                for call in interaction_data:
                    if call not in ["total_prob", "prob", "type", "count", "part"]:
                        node_label += f"<TR><TD>{clear_name(call)}</TD><TD>{interaction_data[call].get_stat():.2f}</TD></TR>"
                node_label += "</TABLE>>"
            graph.node(node_name, label=node_label, color=color, shape='box')

            edge_label = f"{interaction_data['prob']:.2f} ({interaction_data['total_prob']:.2f})"
            graph.edge(prefix_to_graph[prefix], node_name, label=edge_label)
    
    return graph

# Call example : 
# graph = create_graph("test.json", True, 5, True, False, False)
# graph.render(directory="test", view=True)
#
