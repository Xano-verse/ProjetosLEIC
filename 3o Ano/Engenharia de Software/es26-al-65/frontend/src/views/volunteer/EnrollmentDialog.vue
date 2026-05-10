<template>
  <v-dialog v-model="dialog" persistent width="800">
    <v-card>
      <v-card-title>
        <span class="headline">
          {{
            editEnrollment && editEnrollment.id === null
              ? 'New Application'
              : 'Edit Application'
          }}
        </span>
      </v-card-title>
      <v-card-text>
        <v-form ref="form" lazy-validation>
          <v-row>
            <v-col cols="12">
              <v-textarea
                label="*Motivation"
                :rules="[(v) => !!v || 'Motivation is required']"
                required
                v-model="editEnrollment.motivation"
                data-cy="motivationInput"
                auto-grow
                rows="1"
              ></v-textarea>
            </v-col>
          </v-row>

          <!-- Select Shifts -->
          <v-row>
            <v-col cols="12">
              <!-- editEnrollment.shifts is the place where the selected shifts will be stored -->
              <v-select
                v-model="editEnrollment.shiftIds"
                :items="allShifts"
                :item-text="formatShift"
                item-value="id"
                label="*Select Shifts"
                :rules="[(v) => v.length > 0 || 'At least one shift is required',
                () => !hasOverlappingShifts() || 'Selected shifts cannot have overlapping time periods']"
                data-cy="shiftSelect"
                multiple
                chips
              />
            </v-col>
          </v-row>
        </v-form>
      </v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn
          color="blue-darken-1"
          variant="text"
          @click="$emit('close-enrollment-dialog')"
        >
          Close
        </v-btn>
        <v-btn
          v-if="canSave"
          color="blue-darken-1"
          variant="text"
          @click="updateEnrollment"
          data-cy="saveEnrollment"
        >
          Save
        </v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>
<script lang="ts">
import { Vue, Component, Prop, Model } from 'vue-property-decorator';
import RemoteServices from '@/services/RemoteServices';
import { ISOtoString } from '@/services/ConvertDateService';
import Enrollment from '@/models/enrollment/Enrollment';
import Shift from '@/models/shift/Shift';

@Component({
  methods: { ISOtoString },
})
export default class EnrollmentDialog extends Vue {
  @Model('dialog', Boolean) dialog!: boolean;
  @Prop({ type: Enrollment, required: true }) readonly enrollment!: Enrollment;

  editEnrollment: Enrollment = new Enrollment();
  allShifts: Shift[] = [];

  async created() {

    this.editEnrollment = new Enrollment(this.enrollment);

    //if (this.editEnrollment.activityId !== null) {
    this.allShifts = await RemoteServices.getActivityShifts(Number(this.editEnrollment.activityId));
    //}
  }

  get canSave(): boolean {
    return (
      !!this.editEnrollment.motivation &&
      this.editEnrollment.motivation.length >= 10 &&
      this.editEnrollment.shiftIds.length > 0 &&
      !this.hasOverlappingShifts()
    );
  }

  async updateEnrollment() {
    //editar
    if (
      this.editEnrollment.id !== null &&
      (this.$refs.form as Vue & { validate: () => boolean }).validate()
    ) {
      try {
        const result = await RemoteServices.editEnrollment(
          this.editEnrollment.id,
          this.editEnrollment,
        );
        this.$emit('update-enrollment', result);
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
    }
    //criar
    else if (
      this.editEnrollment.activityId !== null &&
      (this.$refs.form as Vue & { validate: () => boolean }).validate()
    ) {
      try {
        const result = await RemoteServices.createEnrollment(
          this.editEnrollment,
        );
        this.$emit('save-enrollment', result);
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
    }
  }


  showTime(time: string) {
    return ISOtoString(time);
  }

  formatShift(shift: Shift) {
    return `${this.showTime(shift.startTime)} - ${this.showTime(shift.endTime)} | ${shift.location}`;
  }

  hasOverlappingShifts(): boolean {
    const selectedShifts = this.allShifts.filter((shift) =>
      shift.id !== null && this.editEnrollment.shiftIds.includes(shift.id));

    for (let i = 0; i < selectedShifts.length; i++) {
      for (let j = i + 1; j < selectedShifts.length; j++) {
        const shift1 = selectedShifts[i];
        const shift2 = selectedShifts[j];

        const start1 = new Date(shift1.startTime).getTime();
        const end1 = new Date(shift1.endTime).getTime();
        const start2 = new Date(shift2.startTime).getTime();
        const end2 = new Date(shift2.endTime).getTime();

        if (start1 < end2 && start2 < end1) {
          return true;
        }
      }
    }
    return false;
  }

}
</script>

<style scoped lang="scss"></style>
