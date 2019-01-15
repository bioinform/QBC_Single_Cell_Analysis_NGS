import pandas as pd
import numpy as np
import argparse
import os
from jitter import jitter

class SingletRemover:
    def __init__(self, cell_marker_cnts_df, X, Y, Z, S):
        self.cell_marker_cnts_df = cell_marker_cnts_df
        self.X = X # expressThreshold
        self.Y = Y # overpositiveThreshold
        self.Z = Z # celleventThreshold
        self.S = S # singletThreshold
        self.cnt_singlet_filtered_cells = 0
        self.cnt_not_enough_expr = 0
        self.cnt_too_few_pos_markers = 0
        self.cnt_too_many_pos_markers = 0
        self.singlets_removed_df = pd.DataFrame()

    def _is_non_singlet(self, arr):
        """Checks if the input record containing marker counts corresponds to a
           singlet cell based on the following rules:
           1. SUM of all expression_values (i.e, counts) over expressionThreshold > celleventThreshold
           2. Number of expression_values (i.e, counts) over expressionThreshold > singletThreshold
           3. Number of expression_values (i.e, counts) over expressionThreshold < overpositiveThreshold
        This method also updates the counts for various error types.
        :param arr: array of counts indicating expression of various markers
        :returns: TRUE if it is not a singlet and FALSE if it is a singlet
        :rtype: boolean

        """
        is_valid = True
        if not ( sum(arr[arr>=self.X]) > self.Z ):
            is_valid = False
            self.cnt_not_enough_expr += 1
        if not ( len(arr[arr>=self.X]) > self.S ):
            is_valid = False
            self.cnt_too_few_pos_markers += 1
        if not ( len(arr[arr>=self.X]) <= self.Y ):
            is_valid = False
            self.cnt_too_many_pos_markers += 1
        return is_valid

    def remove_singlets(self):
        """Removes the records corresponding to singlets and generates a dataframe with non-singlet marker counts

        """
        self.singlets_removed_df = self.cell_marker_cnts_df[ self.cell_marker_cnts_df.apply(lambda row: self._is_non_singlet(np.array(row[1:])), axis=1) ]
        #self.singlets_removed_df = self.singlets_removed_df.iloc[:,1:]

    def write_fcs_files(self, tab_delimited_file, unjittered_file, jittered_file):
        """Writes tab separated Jittered and Un-jittered files with expression counts for non-singlet cells

        :param tab_delimited_file: Path to store tab-delimited data (The only diff from unjittered file is, this contains cell-id)
        :param unjittered_file: Path to store unjittered data
        :param jittered_file: Path to store jittered data

        """
        self.singlets_removed_df.to_csv(tab_delimited_file, sep="\t", index=False, header=True)
        self.singlets_removed_df.iloc[:,1:].to_csv(unjittered_file, sep="\t", index=False, header=True)
        np.random.seed(100)
        jittered_df = self.singlets_removed_df.iloc[:,1:].apply( lambda x: jitter(x, amount=0.5), axis=0 )
        jittered_df.to_csv(jittered_file, sep="\t", index=False, header=True)

    def write_summary_stats(self, output_folder):
        """Writes the summary statistics to a file named "Summary.txt"

        """
        summaryfile = output_folder + "Summary.txt"
        with open(summaryfile, 'w') as summary_fh:
            summary_fh.write('# expressThreshold: {}, overpositiveThreshold: {}, celleventThreshold: {}, singletThreshold: {} \n'.format(self.X, self.Y, self.Z, self.S))
            summary_fh.write( "Number of Cells Before Singlet Removal: {}\n".format(len(self.cell_marker_cnts_df.index)))
            summary_fh.write( "Number of Cells After Singlet Removal: {}\n".format( len(self.singlets_removed_df.index) ))
            summary_fh.write("Cells with not enough expression: {}\n".format( self.cnt_not_enough_expr ))
            summary_fh.write("Cells with too few positive markers: {}\n".format( self.cnt_too_few_pos_markers ))
            summary_fh.write("Cells with too many positive markers: {}\n".format( self.cnt_too_many_pos_markers ))

def command_line_parser():
    parser = argparse.ArgumentParser(description='Singlets Remover')
    parser.add_argument('--by_FCS', help='byFCS file from cudaparse', required=True, default=None)
    parser.add_argument('--singlet_setting', help='Singlet Settings file', required=True, default=None)
    parser.add_argument('--outFolder', help='OutputFolder Location', required=True, default=None)
    args = vars(parser.parse_args())
    return( args )

if __name__ == "__main__":
    args = command_line_parser()
    output_folder = args['outFolder']
    byfcs_file = args['by_FCS']
    singlet_setting_file = args['singlet_setting']
    # Read the marker counts file
    cell_marker_cnts_df = pd.read_csv( byfcs_file, sep="\t" )
    cols = [c for c in cell_marker_cnts_df.columns if not(c.startswith('Unnamed')) ]
    cell_marker_cnts_df = cell_marker_cnts_df[cols]
    # Read the singlet settings file
    singlet_setting_df = pd.read_csv(singlet_setting_file, sep=":", header=None)
    singlet_setting_df.columns = ['Param', 'Value', 'Desc']
    singlet_setting_df.replace('[\t,$]','', regex=True, inplace=True)
    singlet_setting_dict = singlet_setting_df.set_index('Param')['Value'].to_dict()
    X = int(singlet_setting_dict['expressThreshold'])
    Y = int(singlet_setting_dict['OverPositiveThreshold'])
    Z = int(singlet_setting_dict['celleventThreshold'])
    if 'singletThreshold' in singlet_setting_dict:
       S = int(singlet_setting_dict['singletThreshold'])
    else:
       S = 1
    sr = SingletRemover(cell_marker_cnts_df, X, Y, Z, S)
    sr.remove_singlets()
    sr.write_summary_stats(output_folder)
    tab_delimited_file = output_folder + os.path.splitext(os.path.basename(byfcs_file))[0] + '_' + str(Z) + '_' + str(X) + '_' + str(Y) + '_' + str(S) + '.tab_delimited'
    unjittered_file = output_folder + os.path.splitext(os.path.basename(byfcs_file))[0] + '_' + str(Z) + '_' + str(X) + '_' + str(Y) + '_' + str(S) + '.UNJITTERED_forFCS'
    jittered_file = output_folder + os.path.splitext(os.path.basename(byfcs_file))[0] + '_' + str(Z) + '_' + str(X) + '_' + str(Y) + '_' + str(S) + '.JITTERED_forFCS'
    sr.write_fcs_files(tab_delimited_file, unjittered_file, jittered_file)
